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

#include "gb_system.h"
#include "cpu/cpu.h"
#include "cpu/registers.h"
#include "cpu/opcodes.h"
#include "ppu/ppu.h"
#include "apu/apu.h"
#include "mmu/mmu.h"
#include "joypad.h"
#include "serial.h"
#include "moderndos8x16_ttf.h"
#include <stdio.h>
#include <SDL.h>
#include <SDL_audio.h>
#include <SDL_ttf.h>

#define audio_sample_rate (48000)
#define audio_buffer_size (1024)
#define audio_sample_time (1.0 / (double) audio_sample_rate)

// TODO: Add customizable key mappings

static struct keymap {
    SDL_Scancode gb_left;
    SDL_Scancode gb_right;
    SDL_Scancode gb_up;
    SDL_Scancode gb_down;
    SDL_Scancode gb_a;
    SDL_Scancode gb_b;
    SDL_Scancode gb_select;
    SDL_Scancode gb_start;
    SDL_Scancode emu_pause;
    SDL_Scancode emu_speed;
    SDL_Scancode emu_slow;
    SDL_Scancode emu_debug;
    SDL_Scancode emu_exit;
    SDL_Scancode emu_zoom_in;
    SDL_Scancode emu_zoom_out;
    SDL_Scancode emu_vol_up;
    SDL_Scancode emu_vol_down;
} keymap = {
    .gb_left       = SDL_SCANCODE_A,
    .gb_right      = SDL_SCANCODE_D,
    .gb_up         = SDL_SCANCODE_Z,
    .gb_down       = SDL_SCANCODE_S,
    .gb_a          = SDL_SCANCODE_L,
    .gb_b          = SDL_SCANCODE_K,
    .gb_select     = SDL_SCANCODE_RSHIFT,
    .gb_start      = SDL_SCANCODE_RETURN,
    .emu_pause     = SDL_SCANCODE_SPACE,
    .emu_speed     = SDL_SCANCODE_N,
    .emu_slow      = SDL_SCANCODE_H,
    .emu_debug     = SDL_SCANCODE_B,
    .emu_exit      = SDL_SCANCODE_ESCAPE,
    .emu_zoom_in   = SDL_SCANCODE_9,
    .emu_zoom_out  = SDL_SCANCODE_8,
    .emu_vol_up    = SDL_SCANCODE_7,
    .emu_vol_down  = SDL_SCANCODE_6
};

static TTF_Font *font = NULL;

static const int debug_win_width = 500;
static const int debug_win_height = 500;
static SDL_Window *debug_win = NULL;
static uint32_t debug_win_id = 0;
static SDL_Renderer *debug_ren = NULL;

static SDL_Window *lcd_win = NULL;
static uint32_t lcd_win_id = 0;
static SDL_Surface *lcd_surface = NULL;
static uint32_t lcd_pixel_size = 4;
static uint32_t *lcd_pixels = NULL;
static double lcd_win_clock_freq = 0.0;
static double lcd_win_clock_speed = 0.0;
static uint32_t lcd_win_framerate = 0;

static bool stop_emulation = false;
static bool pause_emulation = false;
static uint32_t frames_per_second = 0;

static size_t clocks_per_second = 0;
static uint32_t clock_speed = CPU_CLOCK_SPEED; // 4.194304 MHz
static uint32_t frameskip = 0;

static int audio_pos = 0;
static double audio_volume = 0.5;
static const double audio_volume_step = 0.05;
#define audio_amp (audio_volume * 0.5)

static double audio_time(void)
{
    static double atime = 0.0;
    return (atime += audio_sample_time);
}

// Update window title
void update_window_title(SDL_Window *win, const char *format, ...)
{
    char title[256];
    va_list ap;

    va_start(ap, format);
    vsnprintf(title, sizeof(title), format, ap);
    va_end(ap);
    SDL_SetWindowTitle(win, title);
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

// Draw text using *font
void draw_text(SDL_Renderer *ren, int x, int y, const char *format, ...)
{
    SDL_Surface *surface;
    SDL_Texture *texture;
    SDL_Rect rect;
    char text[256];
    va_list ap;

    va_start(ap, format);
    vsnprintf(text, sizeof(text), format, ap);
    va_end(ap);
    surface = TTF_RenderText_Solid(font, text, (SDL_Color) {0, 0, 0, 255});
    texture = SDL_CreateTextureFromSurface(ren, surface);
    rect.x = x;
    rect.y = y;
    TTF_SizeText(font, text, &rect.w, &rect.h);
    SDL_RenderCopy(ren, texture, NULL, &rect);
    SDL_FreeSurface(surface);
    SDL_DestroyTexture(texture);
}

// Render the debug panel
void render_debug(gb_system_t *gb)
{
    int coll = 5;
    int colr = coll + ((debug_win_width - 5) / 2);
    int w4 = (debug_win_width - 5) / 4;
    int w3 = (debug_win_width - 5) / 3;
    int line_height = TTF_FontHeight(font);
    const opcode_t *opcode;
    byte_t pc_byte;

    if ((pc_byte = mmu_readb_nolog(gb->pc, gb)) == 0xCB) {
        opcode = opcode_cb_identify(mmu_readb_nolog(gb->pc + 1, gb));
    } else {
        opcode = opcode_identify(pc_byte);
    }

    SDL_SetRenderDrawColor(debug_ren, 255, 204, 153, 255);
    SDL_RenderClear(debug_ren);
    draw_text(debug_ren, coll, line_height * 0, "Cycle #%lu", gb->cycle_nb);
    draw_text(debug_ren, coll, line_height * 1, "PC: $%04X", gb->pc);
    draw_text(debug_ren, colr, line_height * 1, "SP: $%04X", gb->sp);
    draw_text(debug_ren, coll, line_height * 2, "A: $%02X", reg_readb(REG_A, gb));
    draw_text(debug_ren, colr, line_height * 2, "F: $%02X", reg_readb(REG_F, gb));
    draw_text(debug_ren, coll, line_height * 3, "B: $%02X", reg_readb(REG_B, gb));
    draw_text(debug_ren, colr, line_height * 3, "C: $%02X", reg_readb(REG_C, gb));
    draw_text(debug_ren, coll, line_height * 4, "D: $%02X", reg_readb(REG_D, gb));
    draw_text(debug_ren, colr, line_height * 4, "E: $%02X", reg_readb(REG_E, gb));
    draw_text(debug_ren, coll, line_height * 5, "H: $%02X", reg_readb(REG_H, gb));
    draw_text(debug_ren, colr, line_height * 5, "L: $%02X", reg_readb(REG_L, gb));
    draw_text(debug_ren, coll,          line_height * 6, "Z: %i", reg_flag(FLAG_Z, gb));
    draw_text(debug_ren, coll + w4,     line_height * 6, "H: %i", reg_flag(FLAG_H, gb));
    draw_text(debug_ren, coll + w4 * 2, line_height * 6, "N: %i", reg_flag(FLAG_N, gb));
    draw_text(debug_ren, coll + w4 * 3, line_height * 6, "C: %i", reg_flag(FLAG_C, gb));
    draw_text(debug_ren, coll,          line_height * 7, "IME: %u", gb->interrupts.ime);
    draw_text(debug_ren, coll + w3,     line_height * 7, "IE: $%02X", gb->interrupts.ie_reg);
    draw_text(debug_ren, coll + w3 * 2, line_height * 7, "IF: $%02X", gb->interrupts.if_reg);
    if (gb->halt) draw_text(debug_ren, coll, line_height * 8, "Halted");
    if (gb->stop) draw_text(debug_ren, colr, line_height * 8, "Stopped");
    draw_text(debug_ren, coll, line_height * 10, "Next instruction:");
    if (opcode) {
        draw_text(debug_ren, coll, line_height * 11, opcode->mnemonic);
        draw_text(debug_ren, coll, line_height * 12, opcode->comment);
    } else {
        draw_text(debug_ren, coll, line_height * 11, "Illegal ($%02X)", pc_byte);
    }
    SDL_RenderPresent(debug_ren);
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
    if (e->key.keysym.scancode == keymap.gb_up) {
        joypad_button(BTN_UP, pressed, gb);
    } else if (e->key.keysym.scancode == keymap.gb_down) {
        joypad_button(BTN_DOWN, pressed, gb);
    } else if (e->key.keysym.scancode == keymap.gb_right) {
        joypad_button(BTN_RIGHT, pressed, gb);
    } else if (e->key.keysym.scancode == keymap.gb_left) {
        joypad_button(BTN_LEFT, pressed, gb);
    } else if (e->key.keysym.scancode == keymap.gb_a) {
        joypad_button(BTN_A, pressed, gb);
    } else if (e->key.keysym.scancode == keymap.gb_b) {
        joypad_button(BTN_B, pressed, gb);
    } else if (e->key.keysym.scancode == keymap.gb_select) {
        joypad_button(BTN_SELECT, pressed, gb);
    } else if (e->key.keysym.scancode == keymap.gb_start) {
        joypad_button(BTN_START, pressed, gb);
    }
}

// Open debug window
void debug_open(void)
{
    if (!debug_win && !debug_ren) {
        SDL_CreateWindowAndRenderer(debug_win_width, debug_win_height, 0, &debug_win, &debug_ren);
        if (!debug_win || !debug_ren) {
            fprintf(stderr, "SDL_CreateWindowAndRenderer: %s\n", SDL_GetError());
            if (debug_ren) SDL_DestroyRenderer(debug_ren);
            if (debug_win) SDL_DestroyWindow(debug_win);
            debug_ren = NULL;
            debug_win = NULL;
            return;
        }
        debug_win_id = SDL_GetWindowID(debug_win);
        update_window_title(debug_win, "Debug");
    }
}

// Close debug window
void debug_close(void)
{
    if (debug_win) {
        SDL_DestroyRenderer(debug_ren);
        SDL_DestroyWindow(debug_win);
        debug_ren = NULL;
        debug_win = NULL;
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
        audio_clock_delay = remaining_clocks / audio_buffer_size;
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
                } else if (audio_pos < audio_buffer_size) {
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
    } else {
        update_window_title(lcd_win, "GameBoy (%.06f MHz: %.02f%%, %u fps, volume: %.f%%)",
            lcd_win_clock_freq,
            lcd_win_clock_speed,
            lcd_win_framerate,
            (float) (audio_volume * 100.0));
    }

    if (debug_win)
        render_debug(gb);
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

        if (e.window.windowID == lcd_win_id) {
            switch (e.type) {
                case SDL_WINDOWEVENT:
                    if (e.window.event == SDL_WINDOWEVENT_CLOSE)
                        stop_emulation = true;
                    break;

                case SDL_KEYDOWN:
                    if (e.key.keysym.scancode == keymap.emu_exit) {
                        stop_emulation = true;
                    } else if (e.key.keysym.scancode == keymap.emu_pause) {
                        pause_emulation = !pause_emulation;
                        update_windows(gb);
                    } else if (e.key.keysym.scancode == keymap.emu_speed) {
                        set_clock_speed(CPU_CLOCK_SPEED * 4);
                    } else if (e.key.keysym.scancode == keymap.emu_slow) {
                        set_clock_speed(CPU_CLOCK_SPEED / 4);
                    } else if (e.key.keysym.scancode == keymap.emu_debug) {
                        debug_win ? debug_close() : debug_open();
                    } else if (e.key.keysym.scancode == keymap.emu_zoom_in) {
                        update_pixel_size(lcd_pixel_size + 1);
                    } else if (e.key.keysym.scancode == keymap.emu_zoom_out) {
                        update_pixel_size(lcd_pixel_size - 1);
                    } else if (e.key.keysym.scancode == keymap.emu_vol_up) {
                        if ((audio_volume += audio_volume_step) > 1.0)
                            audio_volume = 1.0;
                        update_windows(gb);
                    } else if (e.key.keysym.scancode == keymap.emu_vol_down) {
                        if (audio_volume <= audio_volume_step) {
                            audio_volume = 0.0;
                        } else {
                            audio_volume -= audio_volume_step;
                        }
                        update_windows(gb);
                    } else {
                        handle_joypad_input(&e, true, gb);
                    }
                    break;

                case SDL_KEYUP:
                    if (e.key.keysym.scancode == keymap.emu_speed || e.key.keysym.scancode == keymap.emu_slow) {
                        set_clock_speed(CPU_CLOCK_SPEED);
                    } else {
                        handle_joypad_input(&e, false, gb);
                    }
                    break;

                default: break;
            }
        } else if (e.window.windowID == debug_win_id) {
            switch (e.type) {
                case SDL_WINDOWEVENT:
                    if (e.window.event == SDL_WINDOWEVENT_CLOSE)
                        debug_close();
                    break;

                default: break;
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

// Main emulator loop (timed using SDL audio callbacks)
void emulator_audio_loop(void *userdata,
                         Uint8 *stream,
                         __attribute__((unused)) int len)
{
    gb_system_t *gb = (gb_system_t *) userdata;
    float *audio_buffer = (float *) stream;

    if (stop_emulation)
        return;

    audio_pos = 0;
    emulate_clocks(gb, audio_buffer);
    handle_events(gb);
    update_windows(gb);

    if (pause_emulation) {
        // Mute the audio when paused
        for (int i = 0; i < audio_buffer_size; ++i)
            audio_buffer[i] = 0;
    } else {
        // Fill any remaining samples
        while (audio_pos < audio_buffer_size)
            audio_buffer[audio_pos++] = (float) apu_generate_sample(audio_time(), audio_amp, gb);
    }
}

// Emulate GameBoy system loaded in *gb
// Takes care of all the SDL initialization and cleanup
// Returns < 0 on initialization error
// Returns 0 on success
// Returns > 0 on error during emulation
int emulate_gameboy(gb_system_t *gb, const bool enable_audio)
{
    SDL_AudioSpec audiospec;
    SDL_RWops *fontrwo;
    Uint32 sdl_init_flags = SDL_INIT_VIDEO | SDL_INIT_EVENTS;

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
        audiospec.samples = audio_buffer_size;
        audiospec.callback = &emulator_audio_loop;
        audiospec.userdata = gb;

        if (SDL_OpenAudio(&audiospec, NULL) < 0) {
            fprintf(stderr, "SDL_OpenAudio: %s\n", SDL_GetError());
            return -1;
        }
        SDL_PauseAudio(1);
    }

    if (!(fontrwo = SDL_RWFromConstMem(ModernDOS8x16_ttf,
                                       ModernDOS8x16_ttf_len))) {
        fprintf(stderr, "SDL_RWFromConstMem: %s\n", SDL_GetError());
        return -1;
    }

    if (!(font = TTF_OpenFontRW(fontrwo, 1, 16))) {
        fprintf(stderr, "TTF_OpenFont: %s\n", TTF_GetError());
        return -1;
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
    lcd_win_id = SDL_GetWindowID(lcd_win);

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
    if (enable_audio) {
        SDL_PauseAudio(0);
        while (!stop_emulation)
            SDL_Delay(200);
        SDL_PauseAudio(1);
    } else {
        emulator_loop(gb);
    }

    printf("Emulation stopped\n");

    if (gb->memory.mbc_battery)
        mmu_battery_save(gb);

    debug_close();
    SDL_DestroyWindow(lcd_win);
    TTF_CloseFont(font);
    TTF_Quit();
    SDL_Quit();
    return 0;
}