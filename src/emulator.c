/*
emulator.c
GameBoy emulator

Copyright (C) 2020 akrocynova

This program is free software: you can redistribute it and/or modify it under
the terms of the GNU General Public License as published by the Free Software
Foundation, either version 3 of the License, or (at your option) any later
version.

This program is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with
this program. If not, see <https://www.gnu.org/licenses/>.
*/

#include "xalloc.h"
#include "emulator_utils.h"
#include "emulator_events.h"
#include "cpu_view.h"
#include "mmu_view.h"
#include "gb_system.h"
#include "cpu/cpu.h"
#include "mmu/mmu.h"
#include "ppu/ppu.h"
#include "apu/apu.h"
#include "joypad.h"
#include "serial.h"
#include <stdio.h>
#include <SDL.h>
#include <SDL_audio.h>
#include <SDL_ttf.h>

#define MIN(x, y) ((x) < (y) ? (x) : (y))
#define SetRenderBackgroundColor(ren) SDL_SetRenderDrawColor(ren, 32, 32, 32, 255)
#define audio_sample_rate        (48000)
#define audio_buffer_samples     (audio_sample_rate / 60)
#define audio_buffer_size        (audio_buffer_samples * sizeof(float))
#define audio_sample_duration    (1.0 / (double) audio_sample_rate)
#define audio_sample_duration_ms (1000.0 / (double) audio_sample_rate)

// TODO: Add customizable key mappings

static const Uint32 target_framerate = 60;
static const Uint32 target_framerate_ticks = 1000 / target_framerate;
static SDL_Window    *lcd_win             = NULL;
static SDL_Renderer  *lcd_ren             = NULL;
static TTF_Font      *lcd_font            = NULL;
static int            lcd_font_height     = 0;
static double         lcd_win_clock_freq  = 0.0;
static double         lcd_win_clock_speed = 0.0;
static uint32_t       lcd_win_framerate   = 0;
static int            lcd_win_width       = 0;
static int            lcd_win_height      = 0;
static SDL_Surface   *screen_surface      = NULL;
static SDL_Rect       screen_dst          = {.x = 0,
                                             .y = 0,
                                             .w = 0,
                                             .h = 0};
static const SDL_Rect screen_src          = {.x = 0,
                                             .y = 0,
                                             .w = SCREEN_WIDTH,
                                             .h = SCREEN_HEIGHT};

static bool     stop_emulation    = false;
static bool     pause_emulation   = false;
static uint32_t frames_per_second = 0;

static size_t   clocks_per_second = 0;
static uint32_t clock_speed       = CPU_CLOCK_SPEED;
static uint32_t frameskip         = 0;

static SDL_AudioDeviceID audio_devid       = 0;
static int               audio_pos         = 0;
static double            audio_prev_volume = 0.5;
static bool              audio_scaled      = false;
static double            audio_volume      = 0.5;
#define audio_volume_step (0.05)
#define audio_amp         (audio_volume * 0.5)
#define audio_muted       (audio_volume <= 0.0)

static char text_input_buffer[128];
static size_t text_input_idx = 0;

// Scale audio volume by percent
static inline void audio_scale(const double percent)
{
    if (!audio_scaled) {
        audio_scaled = true;
        audio_prev_volume = audio_volume;
        audio_volume *= percent;
    }
}

// Restore previous audio
static inline void audio_unscale(void)
{
    if (audio_scaled) {
        audio_scaled = false;
        audio_volume = audio_prev_volume;
    }
}

// Returns the elapsed audio time in seconds since the program start
static double audio_time(void)
{
    static double atime = 0.0;
    return (atime += audio_sample_duration);
}

// Update scales and positions using the window size
void update_window_size()
{
    int scale;

    SDL_GetWindowSize(lcd_win, &lcd_win_width, &lcd_win_height);
    scale = MIN(lcd_win_width / SCREEN_WIDTH, lcd_win_height / SCREEN_HEIGHT);
    if (!scale) scale = 1;
    screen_dst.w = SCREEN_WIDTH * scale;
    screen_dst.h = SCREEN_HEIGHT * scale;
    screen_dst.x = (lcd_win_width - screen_dst.w) / 2;
    screen_dst.y = (lcd_win_height - screen_dst.h) / 2;
}

// Render a frame
void render_frame(__attribute__((unused)) gb_system_t *gb)
{
    SDL_Texture *screen_texture;

    // Clear the window
    SetRenderBackgroundColor(lcd_ren);
    SDL_RenderClear(lcd_ren);

    // Render the Gameboy screen
    screen_texture = SDL_CreateTextureFromSurface(lcd_ren, screen_surface);
    SDL_RenderCopy(lcd_ren, screen_texture, &screen_src, &screen_dst);
    SDL_DestroyTexture(screen_texture);

    if (pause_emulation) {
        render_text_outline(lcd_font, lcd_ren, 3, 3,  "Paused");
    }

    if (serial_connected()) {
        render_text_outline(lcd_font, lcd_ren, 3, lcd_win_height - lcd_font_height - 3,
                            serial_server_opened() ? "Server" : "Client");
        render_text_outline(lcd_font, lcd_ren, 3, lcd_win_height - (lcd_font_height * 2) - 3,
                            serial_remote_address());
        render_text_outline(lcd_font, lcd_ren, 3, lcd_win_height - (lcd_font_height * 3) - 3,
                            "%s (packet id: %u)", gb->serial.sc.internal_clock ? "INT" : "EXT",
                            serial_pkt_id());
    } else if (serial_server_opened()) {
        render_text_outline(lcd_font, lcd_ren, 3, lcd_win_height - lcd_font_height - 3, "Server");
        render_text_outline(lcd_font, lcd_ren, 3, lcd_win_height - (lcd_font_height * 2) - 3, "-");
    }

    if (SDL_IsTextInputActive()) {
        render_text_outline(lcd_font, lcd_ren, 3, 3 + lcd_font_height, "> %s", text_input_buffer);
    }

    // Update display and increment frame counter
    SDL_RenderPresent(lcd_ren);
    frames_per_second += 1;
}

// Update the GameBoy framebuffer to the screen_surface buffer and render frame
void render_framebuffer(gb_system_t *gb)
{
    static uint32_t frameskip_counter = 0;

    if (frameskip_counter == 0) {
        for (byte_t y = 0; y < SCREEN_HEIGHT; ++y) {
            for (byte_t x = 0; x < SCREEN_WIDTH; ++x) {
                ((uint32_t *) screen_surface->pixels)[(x + (y * SCREEN_WIDTH))] =
                      0xff000000
                    | (gb->screen.framebuffer[y][x].r << 16)
                    | (gb->screen.framebuffer[y][x].g << 8)
                    | (gb->screen.framebuffer[y][x].b << 0);
            }
        }

        render_frame(gb);
    }
    if ((++frameskip_counter) > frameskip)
        frameskip_counter = 0;
}

// Change the emulated clock speed and update the frameskip value
void set_clock_speed(uint32_t speed)
{
    if (clock_speed != speed) {
        clocks_per_second = 0;
        clock_speed = speed;
        if (clock_speed >= CPU_CLOCK_SPEED) {
            frameskip = (clock_speed / CPU_CLOCK_SPEED) - 1;
        } else {
            frameskip = 0;
        }
    }
}

// Handle SDL input for the joypad
void handle_joypad_input(SDL_Event *e, const bool pressed, gb_system_t *gb)
{
    if (e->key.keysym.scancode == emu_keymap.gb_up) {
        joypad_button(BTN_UP, pressed, gb);
    } else if (e->key.keysym.scancode == emu_keymap.gb_down) {
        joypad_button(BTN_DOWN, pressed, gb);
    } else if (e->key.keysym.scancode == emu_keymap.gb_right) {
        joypad_button(BTN_RIGHT, pressed, gb);
    } else if (e->key.keysym.scancode == emu_keymap.gb_left) {
        joypad_button(BTN_LEFT, pressed, gb);
    } else if (e->key.keysym.scancode == emu_keymap.gb_a) {
        joypad_button(BTN_A, pressed, gb);
    } else if (e->key.keysym.scancode == emu_keymap.gb_b) {
        joypad_button(BTN_B, pressed, gb);
    } else if (e->key.keysym.scancode == emu_keymap.gb_select) {
        joypad_button(BTN_SELECT, pressed, gb);
    } else if (e->key.keysym.scancode == emu_keymap.gb_start) {
        joypad_button(BTN_START, pressed, gb);
    }
}

// Emulate clocks
void emulate_clocks(gb_system_t *gb, float *audio_buffer)
{
    static Uint32 last_ticks = 0;
    static double second_elapsed = 0.0;
    Uint32 ticks;
    size_t audio_remaining_clocks;
    size_t audio_clock_delay;
    size_t lfsr_remaining;
    size_t lfsr_delay;
    size_t remaining_clocks;
    double elapsed;

    // Initialize last_ticks
    if (last_ticks == 0)
        last_ticks = SDL_GetTicks();

    // Get current ticks
    ticks = SDL_GetTicks();

    // Calculate elapsed time since last call to emulate_clocks()
    elapsed = (double) (ticks - last_ticks) / 1000.0;

    // Refresh counters every second
    if ((second_elapsed += elapsed) >= 1.0) {
        second_elapsed = 0.0;

        lcd_win_clock_freq = (double) clocks_per_second / (double) 1000000.0;
        lcd_win_clock_speed = (double) clocks_per_second / (double) CPU_CLOCK_SPEED * 100.0;
        lcd_win_framerate = frames_per_second;

        clocks_per_second = 0;
        frames_per_second = 0;
    }

    if (!pause_emulation) {
        // Calculate how many clocks should be emulated
        // since last frame
        remaining_clocks = elapsed * clock_speed;

        // Never exceed the clock speed
        if (clocks_per_second <= clock_speed && clocks_per_second + remaining_clocks > clock_speed)
            remaining_clocks = clock_speed - clocks_per_second;

        // Calculate when to generate an audio sample
        audio_clock_delay = remaining_clocks / audio_buffer_samples;
        audio_remaining_clocks = 0;

        // Calculate when to clock LFSR
        lfsr_remaining = (size_t) (gb->apu.ch4.freq * elapsed);
        lfsr_delay = (lfsr_remaining > 0) ? (remaining_clocks / lfsr_remaining) : 0;
        lfsr_remaining = 0;

        // Count the clocks we are about to emulate
        clocks_per_second += remaining_clocks;

        // Emulate the clocks
        for (; remaining_clocks > 0; --remaining_clocks, gb->cycle_nb += 1) {
            if (cpu_cycle(gb) < 0) {
                // Emulation should be stopped
                stop_emulation = true;
                break;
            }
            ppu_cycle(gb);
            serial_cycle(gb);

            if (gb->memory.mbc_clock)
                (*(gb->memory.mbc_clock))(gb);

            if (audio_buffer) {
                // Only generate audio samples if an audio_buffer is given
                if (audio_remaining_clocks > 0) {
                    audio_remaining_clocks -= 1;
                } else if (audio_pos < audio_buffer_samples) {
                    audio_remaining_clocks = audio_clock_delay;
                    audio_buffer[audio_pos++] = (float) apu_generate_sample(audio_time(), audio_amp, gb);
                }
            }

            if (lfsr_remaining > 0) {
                lfsr_remaining -= 1;
            } else if (lfsr_delay > 0) {
                lfsr_remaining = lfsr_delay;
                apu_lfsr_clock(gb);
            }
        }
    }

    last_ticks = ticks;
}

void update_windows(gb_system_t *gb)
{
    if (audio_devid == 0) {
        update_window_title(lcd_win, "GameBoy (%.06f MHz: %.02f%%, %u fps, audio off)",
            lcd_win_clock_freq,
            lcd_win_clock_speed,
            lcd_win_framerate);
    } else if (audio_muted) {
        update_window_title(lcd_win, "GameBoy (%.06f MHz: %.02f%%, %u fps, volume: muted)",
            lcd_win_clock_freq,
            lcd_win_clock_speed,
            lcd_win_framerate);
    } else {
        update_window_title(lcd_win, "GameBoy (%.06f MHz: %.02f%%, %u fps, volume: %.f%%)",
            lcd_win_clock_freq,
            lcd_win_clock_speed,
            lcd_win_framerate,
            (float) (audio_volume * 100.0));
    }

    if (emu_windows[EMU_WINDOWS_CPU_VIEW].win)
        cpu_view_render(gb);
    if (emu_windows[EMU_WINDOWS_MMU_VIEW].win)
        mmu_view_render(gb);
}

// Handle main window event
void lcd_event(SDL_Event *e, gb_system_t *gb)
{
    switch (e->type) {
        case SDL_WINDOWEVENT:
            switch (e->window.event) {
                case SDL_WINDOWEVENT_CLOSE:
                    stop_emulation = true;
                    break;

                case SDL_WINDOWEVENT_RESIZED:
                    update_window_size();
                    break;

                default: break;
            }
            break;

        case SDL_TEXTINPUT:
            for (size_t i = 0; e->text.text[i] && text_input_idx < (sizeof(text_input_buffer) - 1); ++i)
                text_input_buffer[text_input_idx++] = e->text.text[i];
            break;

        case SDL_KEYDOWN:
            if (SDL_IsTextInputActive()) {
                if (e->key.keysym.scancode == SDL_SCANCODE_RETURN) {
                    SDL_StopTextInput();
                    serial_connect(text_input_buffer);
                } else if (e->key.keysym.scancode == SDL_SCANCODE_ESCAPE) {
                    SDL_StopTextInput();
                } else if (e->key.keysym.scancode == SDL_SCANCODE_BACKSPACE) {
                    if (text_input_idx == 0) {
                        SDL_StopTextInput();
                    } else {
                        text_input_buffer[--text_input_idx] = '\0';
                    }
                }
            } else if (e->key.keysym.scancode == emu_keymap.emu_exit) {
                stop_emulation = true;
            } else if (e->key.keysym.scancode == emu_keymap.emu_pause) {
                pause_emulation = !pause_emulation;
                update_windows(gb);
            } else if (e->key.keysym.scancode == emu_keymap.emu_speed) {
                set_clock_speed(CPU_CLOCK_SPEED * 4);
                audio_scale(0.2);
            } else if (e->key.keysym.scancode == emu_keymap.emu_slow) {
                set_clock_speed(CPU_CLOCK_SPEED / 4);
                audio_scale(0.2);
            } else if (e->key.keysym.scancode == emu_keymap.emu_vol_up) {
                if (!audio_scaled) {
                    if ((audio_volume += audio_volume_step) >= 1.0)
                        audio_volume = 1.0;
                    update_windows(gb);
                }
            } else if (e->key.keysym.scancode == emu_keymap.emu_vol_down) {
                if (!audio_scaled) {
                    if ((audio_volume -= audio_volume_step) <= 0.001)
                        audio_volume = 0.0;
                    update_windows(gb);
                }
            } else if (e->key.keysym.scancode == emu_keymap.emu_cpu_view) {
                if (emu_windows[EMU_WINDOWS_CPU_VIEW].win) {
                    cpu_view_close();
                } else {
                    cpu_view_open();
                }
            } else if (e->key.keysym.scancode == emu_keymap.emu_mmu_view) {
                if (emu_windows[EMU_WINDOWS_MMU_VIEW].win) {
                    mmu_view_close();
                } else {
                    mmu_view_open();
                }
            } else if (e->key.keysym.scancode == emu_keymap.link_cable_server) {
                if (serial_server_opened()) {
                    serial_server_close();
                } else {
                    serial_server_open();
                }
            } else if (e->key.keysym.scancode == emu_keymap.link_cable_client) {
                if (serial_connected()) {
                    serial_disconnect(gb);
                } else if (!SDL_IsTextInputActive()) {
                    memset(text_input_buffer, 0, sizeof(text_input_buffer));
                    text_input_idx = 0;
                    SDL_StartTextInput();
                }
            } else {
                handle_joypad_input(e, true, gb);
            }
            break;

        case SDL_KEYUP:
            if (SDL_IsTextInputActive())
                break;

            if (e->key.keysym.scancode == emu_keymap.emu_speed || e->key.keysym.scancode == emu_keymap.emu_slow) {
                set_clock_speed(CPU_CLOCK_SPEED);
                audio_unscale();
            } else {
                handle_joypad_input(e, false, gb);
            }
            break;

        default: break;
    }
}

// Handle SDL events
void handle_events(gb_system_t *gb)
{
    SDL_Event e;

    serial_server_accept();
    while (SDL_PollEvent(&e)) {
        if (e.type == SDL_QUIT) {
            stop_emulation = true;
            return;
        }

        for (int i = 0; i < EMU_WINDOWS_SIZE; ++i) {
            if (emu_windows[i].win && emu_windows[i].id == e.window.windowID) {
                if (emu_windows[i].handle_event)
                    (*(emu_windows[i].handle_event))(&e, gb);
            }
        }
    }
}

// Main emulator loop (timed using elapsed time in milliseconds)
int emulator_loop(gb_system_t *gb)
{
    Uint32 frame_start;
    Uint32 frame_ticks;

    while (!stop_emulation) {
        frame_start = SDL_GetTicks();

        emulate_clocks(gb, NULL);
        handle_events(gb);
        update_windows(gb);

        // Calculate elapsed time in ms for a single frame
        frame_ticks = SDL_GetTicks() - frame_start;

        // If emulation is paused, render frames manually
        if (pause_emulation)
            render_frame(gb);

        // If a single frame takes less time than frame_ms, add a delay
        // to reach the target_framerate
        if (!stop_emulation && frame_ticks < target_framerate_ticks)
            SDL_Delay(target_framerate_ticks - frame_ticks);
    }

    return 0;
}

// Main emulator loop (timed using audio queue)
int emulator_audio_loop(gb_system_t *gb)
{
    float *audio_buffer = xalloc(audio_buffer_size);
    Uint32 audio_pending, audio_delay;
    Uint32 next_frame = 0;

    SDL_PauseAudioDevice(audio_devid, 0);
    while (!stop_emulation) {
        audio_pos = 0;
        emulate_clocks(gb, audio_buffer);
        handle_events(gb);
        update_windows(gb);

        if (pause_emulation) {
            // Mute the audio when paused
            for (int i = 0; i < audio_buffer_samples; ++i)
                audio_buffer[i] = 0;

            // If emulation is paused, render frames manually
            if (SDL_GetTicks() >= next_frame) {
                next_frame = SDL_GetTicks() + target_framerate_ticks;
                render_frame(gb);
            }
        } else {
            // Fill any remaining samples
            while (audio_pos < audio_buffer_samples)
                audio_buffer[audio_pos++] = (float) apu_generate_sample(audio_time(), audio_amp, gb);
        }

        // Wait for the queue to be empty before queuing more samples
        while ((audio_pending = SDL_GetQueuedAudioSize(audio_devid)) > 0) {
            audio_delay = (audio_sample_duration_ms * (audio_pending / sizeof(float))) / 4;
            SDL_Delay(audio_delay);
            handle_events(gb);
        }
        SDL_QueueAudio(audio_devid, audio_buffer, audio_buffer_size);
    }

    free(audio_buffer);
    return 0;
}

// Emulate GameBoy system loaded in *gb
// Returns < 0 on initialization error
// Returns 0 on success
// Returns > 0 on error during emulation
int emulate_gameboy(gb_system_t *gb, bool enable_audio)
{
    SDL_AudioSpec audiospec;

    if (!(lcd_font = load_default_font()))
        return -1;
    lcd_font_height = TTF_FontHeight(lcd_font);

    memset(emu_windows, 0, EMU_WINDOWS_SIZE * sizeof(struct emu_windows));
    if (enable_audio) {
        if (SDL_Init(SDL_INIT_AUDIO) < 0) {
            fprintf(stderr, "SDL audio initialization failed: %s\n", SDL_GetError());
        } else {
            audiospec.freq = audio_sample_rate;
            audiospec.format = AUDIO_F32SYS;
            audiospec.channels = 1;
            audiospec.samples = audio_buffer_samples;
            audiospec.callback = NULL;
            audiospec.userdata = gb;

            if (!(audio_devid = SDL_OpenAudioDevice(NULL, 0, &audiospec, NULL, 0))) {
                fprintf(stderr, "SDL_OpenAudioDevice: %s\n", SDL_GetError());
            }
        }
    }

    if (SDL_CreateWindowAndRenderer(SCREEN_WIDTH * 4,
                                    SCREEN_HEIGHT * 4,
                                    SDL_WINDOW_RESIZABLE,
                                    &lcd_win, &lcd_ren) < 0)
    {
        fprintf(stderr, "SDL_CreateWindowAndRenderer: %s\n", SDL_GetError());
        return -1;
    }

    emu_windows_set(EMU_WINDOWS_LCD, lcd_win, &lcd_event);
    update_window_title(lcd_win, "GameBoy");
    update_window_size();

    if (!(screen_surface = SDL_CreateRGBSurface(0,
                                                SCREEN_WIDTH, SCREEN_HEIGHT,
                                                32,
                                                0x00ff0000, 0x0000ff00,
                                                0x000000ff, 0xff000000)))
    {
        fprintf(stderr, "SDL_CreateRGBSurface: %s\n", SDL_GetError());
        return -1;
    }

    SetRenderBackgroundColor(lcd_ren);
    SDL_RenderClear(lcd_ren);
    SDL_RenderPresent(lcd_ren);
    gb->screen.vblank_callback = &render_framebuffer;

    if (gb->memory.mbc_battery)
        mmu_battery_load(gb);

    SDL_StopTextInput();
    printf("Emulating: %s\n", gb->cartridge.title);
    if (audio_devid) {
        apu_initialize(audio_sample_rate, gb);
        emulator_audio_loop(gb);
    } else {
        emulator_loop(gb);
    }
    printf("Emulation stopped\n");

    if (gb->memory.mbc_battery)
        mmu_battery_save(gb);

    cpu_view_close();
    mmu_view_close();
    SDL_DestroyRenderer(lcd_ren);
    SDL_DestroyWindow(lcd_win);
    SDL_FreeSurface(screen_surface);
    TTF_CloseFont(lcd_font);
    return 0;
}