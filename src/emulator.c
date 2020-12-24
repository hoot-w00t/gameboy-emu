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

#define audio_sample_rate        (48000)
#define audio_buffer_samples     (audio_sample_rate / 60)
#define audio_buffer_size        (audio_buffer_samples * sizeof(float))
#define audio_sample_duration    (1.0 / (double) audio_sample_rate)
#define audio_sample_duration_ms (1000.0 / (double) audio_sample_rate)

// TODO: Add customizable key mappings

static SDL_Window  *lcd_win             = NULL;
static SDL_Surface *lcd_surface         = NULL;
static uint32_t     lcd_pixel_size      = 4;
static uint32_t    *lcd_pixels          = NULL;
static double       lcd_win_clock_freq  = 0.0;
static double       lcd_win_clock_speed = 0.0;
static uint32_t     lcd_win_framerate   = 0;

static bool     stop_emulation    = false;
static bool     pause_emulation   = false;
static uint32_t frames_per_second = 0;

static size_t   clocks_per_second = 0;
static uint32_t clock_speed       = CPU_CLOCK_SPEED;
static uint32_t frameskip         = 0;

static SDL_AudioDeviceID audio_devid       = 0;
static int               audio_pos         = 0;
static bool              audio_mute        = false;
static double            audio_volume      = 0.5;
static const double      audio_volume_step = 0.05;
#define audio_amp       (audio_volume * 0.5)

// Returns the elapsed audio time in seconds since the program start
static double audio_time(void)
{
    static double atime = 0.0;
    return (atime += audio_sample_duration);
}

// Update pixel size, resize window and update surface
void update_pixel_size(uint32_t size)
{
    if (size > 0 && size < 8) {
        lcd_pixel_size = size;
        SDL_SetWindowSize(lcd_win,
            SCREEN_WIDTH * size,
            SCREEN_HEIGHT * size);

        if (!(lcd_surface = SDL_GetWindowSurface(lcd_win))) {
            fprintf(stderr, "SDL_GetWindowSurface: %s\n", SDL_GetError());
            stop_emulation = true;
            return;
        }
        lcd_pixels = (uint32_t *) lcd_surface->pixels;
    }
}

// Render the GameBoy framebuffer to the lcd_win framebuffer
void render_framebuffer(gb_system_t *gb)
{
    static uint32_t frameskip_counter = 0;
    uint32_t line_width = SCREEN_WIDTH * lcd_pixel_size;
    uint32_t line_height = line_width * lcd_pixel_size;
    uint32_t offset;

    if (frameskip_counter == 0) {
        for (byte_t y = 0; y < SCREEN_HEIGHT; ++y) {
            for (byte_t x = 0; x < SCREEN_WIDTH; ++x) {
                offset = (x * lcd_pixel_size) + (y * line_height);

                for (byte_t sy = 0; sy < lcd_pixel_size; ++sy) {
                    for (byte_t sx = 0; sx < lcd_pixel_size; ++sx) {
                        lcd_pixels[offset + sx + (sy * line_width)] = 0xff000000 |
                            (gb->screen.framebuffer[y][x].r << 16) |
                            (gb->screen.framebuffer[y][x].g << 8) |
                            gb->screen.framebuffer[y][x].b;
                    }
                }
            }
        }
        frames_per_second += 1;
        SDL_UpdateWindowSurface(lcd_win);
    }
    if ((++frameskip_counter) >= frameskip)
        frameskip_counter = 0;
}

// Change the emulated clock speed
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
    Uint32 ticks = SDL_GetTicks();
    size_t audio_remaining_clocks;
    size_t audio_clock_delay;
    size_t lfsr_remaining;
    size_t lfsr_delay;
    size_t remaining_clocks;
    double elapsed;

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
        for (; remaining_clocks > 0; --remaining_clocks) {
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
    if (pause_emulation) {
        update_window_title(lcd_win, "GameBoy (paused)");
    } else if (audio_devid == 0) {
        update_window_title(lcd_win, "GameBoy (%.06f MHz: %.02f%%, %u fps, audio off)",
            lcd_win_clock_freq,
            lcd_win_clock_speed,
            lcd_win_framerate);
    } else if (audio_mute) {
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
            if (e->window.event == SDL_WINDOWEVENT_CLOSE)
                stop_emulation = true;
            break;

        case SDL_KEYDOWN:
            if (e->key.keysym.scancode == emu_keymap.emu_exit) {
                stop_emulation = true;
            } else if (e->key.keysym.scancode == emu_keymap.emu_pause) {
                pause_emulation = !pause_emulation;
                update_windows(gb);
            } else if (e->key.keysym.scancode == emu_keymap.emu_speed) {
                set_clock_speed(CPU_CLOCK_SPEED * 4);
            } else if (e->key.keysym.scancode == emu_keymap.emu_slow) {
                set_clock_speed(CPU_CLOCK_SPEED / 4);
            } else if (e->key.keysym.scancode == emu_keymap.emu_zoom_in) {
                update_pixel_size(lcd_pixel_size + 1);
            } else if (e->key.keysym.scancode == emu_keymap.emu_zoom_out) {
                update_pixel_size(lcd_pixel_size - 1);
            } else if (e->key.keysym.scancode == emu_keymap.emu_vol_up) {
                if ((audio_volume += audio_volume_step) >= 1.0)
                    audio_volume = 1.0;
                update_windows(gb);
            } else if (e->key.keysym.scancode == emu_keymap.emu_vol_down) {
                if ((audio_volume -= audio_volume_step) <= audio_volume_step)
                    audio_volume = 0.0;
                update_windows(gb);
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
            } else {
                handle_joypad_input(e, true, gb);
            }
            break;

        case SDL_KEYUP:
            if (e->key.keysym.scancode == emu_keymap.emu_speed || e->key.keysym.scancode == emu_keymap.emu_slow) {
                set_clock_speed(CPU_CLOCK_SPEED);
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

// Main emulator loop (timed using framerate)
int emulator_loop(gb_system_t *gb)
{
    Uint32 target_framerate = 60;
    Uint32 frame_ms = 1000 / target_framerate;
    Uint32 frame_start;
    Uint32 frame_time;

    while (!stop_emulation) {
        frame_start = SDL_GetTicks();

        emulate_clocks(gb, NULL);
        handle_events(gb);
        update_windows(gb);

        // Calculate elapsed time in ms for a single frame
        frame_time = SDL_GetTicks() - frame_start;

        // If a single frame takes less time than frame_ms, add a delay
        // to reach the target_framerate
        if (!stop_emulation && frame_time < frame_ms)
            SDL_Delay(frame_ms - frame_time);
    }

    return 0;
}

// Main emulator loop (timed using audio queue)
int emulator_audio_loop(gb_system_t *gb)
{
    float *audio_buffer = xalloc(audio_buffer_size);
    Uint32 audio_pending, audio_delay;

    SDL_PauseAudioDevice(audio_devid, 0);
    while (!stop_emulation) {
        audio_mute = (clock_speed > CPU_CLOCK_SPEED) || (audio_volume <= 0.0);
        audio_pos = 0;
        emulate_clocks(gb, audio_buffer);
        handle_events(gb);
        update_windows(gb);

        if (!pause_emulation) {
            // Fill any remaining samples
            while (audio_pos < audio_buffer_samples)
                audio_buffer[audio_pos++] = (float) apu_generate_sample(audio_time(), audio_amp, gb);
        }

        if (pause_emulation || audio_mute) {
            // Mute the audio when paused
            memset(audio_buffer, 0, audio_buffer_size);
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
// Takes care of all the SDL initialization and cleanup
// Returns < 0 on initialization error
// Returns 0 on success
// Returns > 0 on error during emulation
int emulate_gameboy(gb_system_t *gb, bool enable_audio)
{
    SDL_AudioSpec audiospec;
    Uint32 sdl_init_flags = SDL_INIT_VIDEO | SDL_INIT_EVENTS;

    memset(emu_windows, 0, EMU_WINDOWS_SIZE * sizeof(struct emu_windows));
    if (enable_audio)
        sdl_init_flags |= SDL_INIT_AUDIO;

    if (SDL_Init(sdl_init_flags) < 0) {
        fprintf(stderr, "SDL initialization failed: %s\n", SDL_GetError());
        return -1;
    }

    if (TTF_Init() < 0) {
        fprintf(stderr, "SDL_ttf initialization failed: %s\n", TTF_GetError());
        return -1;
    }

    if (enable_audio) {
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

    if (!(lcd_win = SDL_CreateWindow("GameBoy",
            SDL_WINDOWPOS_UNDEFINED,
            SDL_WINDOWPOS_UNDEFINED,
            SCREEN_WIDTH * lcd_pixel_size,
            SCREEN_HEIGHT * lcd_pixel_size,
            0))) {
        fprintf(stderr, "SDL_CreateWindow: %s\n", SDL_GetError());
        return -1;
    }
    emu_windows_set(EMU_WINDOWS_LCD, lcd_win, &lcd_event);

    if (!(lcd_surface = SDL_GetWindowSurface(lcd_win))) {
        fprintf(stderr, "SDL_GetWindowSurface: %s\n", SDL_GetError());
        return -1;
    }
    lcd_pixels = (uint32_t *) lcd_surface->pixels;
    SDL_UpdateWindowSurface(lcd_win);
    gb->screen.vblank_callback = &render_framebuffer;

    if (gb->memory.mbc_battery)
        mmu_battery_load(gb);

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
    SDL_DestroyWindow(lcd_win);
    TTF_Quit();
    SDL_Quit();
    return 0;
}