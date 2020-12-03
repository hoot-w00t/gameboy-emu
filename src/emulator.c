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
#include "mmu/mmu.h"
#include "joypad.h"
#include "serial.h"
#include "moderndos8x16_ttf.h"
#include <stdio.h>
#include <SDL.h>
#include <SDL_ttf.h>

// TODO: Use a proper pixel size variable
#define SDL_PIXEL_SIZE (4)
static TTF_Font *font = NULL;
static SDL_Rect debug_rect;
static bool debug_enabled = false;

// Create SDL window
int create_window(SDL_Window **win, SDL_Renderer **ren)
{
    SDL_CreateWindowAndRenderer(
        SCREEN_WIDTH * SDL_PIXEL_SIZE + debug_rect.w,
        SCREEN_HEIGHT * SDL_PIXEL_SIZE,
        0, win, ren);

    if (*win == NULL || *ren == NULL) {
        fprintf(stderr, "Unable to create SDL window: %s\n", SDL_GetError());
        return -1;
    }

    return 0;
}

void debug_enable(SDL_Window *win)
{
    debug_enabled = true;
    debug_rect.w = 300;
    SDL_SetWindowSize(win,
                      SCREEN_WIDTH * SDL_PIXEL_SIZE + debug_rect.w,
                      SCREEN_HEIGHT * SDL_PIXEL_SIZE);
}

void debug_disable(SDL_Window *win)
{
    debug_enabled = false;
    debug_rect.w = 0;
    SDL_SetWindowSize(win,
                      SCREEN_WIDTH * SDL_PIXEL_SIZE + debug_rect.w,
                      SCREEN_HEIGHT * SDL_PIXEL_SIZE);
}

// Update SDL window title
void update_window_title(SDL_Window *win, const char *format, ...)
{
    char title[256];
    va_list ap;

    va_start(ap, format);
    vsnprintf(title, sizeof(title), format, ap);
    va_end(ap);
    SDL_SetWindowTitle(win, title);
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
void render_debug(SDL_Renderer *ren, gb_system_t *gb)
{
    int coll = debug_rect.x + 5;
    int colr = coll + ((debug_rect.w - 5) / 2);
    int w4 = (debug_rect.w - 5) / 4;
    int w3 = (debug_rect.w - 5) / 3;
    int line_height = TTF_FontHeight(font);
    const opcode_t *opcode;
    byte_t pc_byte;

    if ((pc_byte = mmu_readb_nolog(gb->pc, gb)) == 0xCB) {
        opcode = opcode_cb_identify(mmu_readb_nolog(gb->pc + 1, gb));
    } else {
        opcode = opcode_identify(pc_byte);
    }

    SDL_SetRenderDrawColor(ren, 255, 204, 153, 255);
    SDL_RenderFillRect(ren, &debug_rect);
    draw_text(ren, coll, line_height * 0, "Cycle #%lu", gb->cycle_nb);
    draw_text(ren, coll, line_height * 1, "PC: $%04X", gb->pc);
    draw_text(ren, colr, line_height * 1, "SP: $%04X", gb->sp);
    draw_text(ren, coll, line_height * 2, "A: $%02X", reg_readb(REG_A, gb));
    draw_text(ren, colr, line_height * 2, "F: $%02X", reg_readb(REG_F, gb));
    draw_text(ren, coll, line_height * 3, "B: $%02X", reg_readb(REG_B, gb));
    draw_text(ren, colr, line_height * 3, "C: $%02X", reg_readb(REG_C, gb));
    draw_text(ren, coll, line_height * 4, "D: $%02X", reg_readb(REG_D, gb));
    draw_text(ren, colr, line_height * 4, "E: $%02X", reg_readb(REG_E, gb));
    draw_text(ren, coll, line_height * 5, "H: $%02X", reg_readb(REG_H, gb));
    draw_text(ren, colr, line_height * 5, "L: $%02X", reg_readb(REG_L, gb));
    draw_text(ren, coll,          line_height * 6, "Z: %i", reg_flag(FLAG_Z, gb));
    draw_text(ren, coll + w4,     line_height * 6, "H: %i", reg_flag(FLAG_H, gb));
    draw_text(ren, coll + w4 * 2, line_height * 6, "N: %i", reg_flag(FLAG_N, gb));
    draw_text(ren, coll + w4 * 3, line_height * 6, "C: %i", reg_flag(FLAG_C, gb));
    draw_text(ren, coll,          line_height * 7, "IME: %u", gb->interrupts.ime);
    draw_text(ren, coll + w3,     line_height * 7, "IE: $%02X", gb->interrupts.ie_reg);
    draw_text(ren, coll + w3 * 2, line_height * 7, "IF: $%02X", gb->interrupts.if_reg);
    if (gb->halt) draw_text(ren, coll, line_height * 8, "Halted");
    if (gb->stop) draw_text(ren, colr, line_height * 8, "Stopped");
    draw_text(ren, coll, line_height * 10, "Next instruction:");
    if (opcode) {
        draw_text(ren, coll, line_height * 11, opcode->mnemonic);
        draw_text(ren, coll, line_height * 12, opcode->comment);
    } else {
        draw_text(ren, coll, line_height * 11, "Illegal ($%02X)", pc_byte);
    }
}

// Draw a scaled pixel
static inline void draw_pixel(SDL_Renderer *ren,
    int y, int x, int size, struct pixel *pixel)
{
    SDL_Rect rect;

    rect.h = rect.w = size;
    rect.x = x * size;
    rect.y = y * size;

    SDL_SetRenderDrawColor(ren, pixel->r, pixel->g, pixel->b, 255);
    SDL_RenderFillRect(ren, &rect);
}

// Render the gb->screen.framebuffer
void render_framebuffer(SDL_Renderer *ren, gb_system_t *gb)
{
    for (uint16_t y = 0; y < SCREEN_HEIGHT; ++y) {
        for (uint16_t x = 0; x < SCREEN_WIDTH; ++x) {
            draw_pixel(ren, y, x, SDL_PIXEL_SIZE, &gb->screen.framebuffer[y][x]);
        }
    }
}

void handle_joypad_input(SDL_Event *e, const bool pressed, gb_system_t *gb)
{
    if (e->key.keysym.sym == SDLK_UP) {
        joypad_button(BTN_UP, pressed, gb);
    } else if (e->key.keysym.sym == SDLK_DOWN) {
        joypad_button(BTN_DOWN, pressed, gb);
    } else if (e->key.keysym.sym == SDLK_RIGHT) {
        joypad_button(BTN_RIGHT, pressed, gb);
    } else if (e->key.keysym.sym == SDLK_LEFT) {
        joypad_button(BTN_LEFT, pressed, gb);
    } else if (e->key.keysym.sym == SDLK_a) {
        joypad_button(BTN_A, pressed, gb);
    } else if (e->key.keysym.sym == SDLK_b) {
        joypad_button(BTN_B, pressed, gb);
    } else if (e->key.keysym.sym == SDLK_o) {
        joypad_button(BTN_SELECT, pressed, gb);
    } else if (e->key.keysym.sym == SDLK_p) {
        joypad_button(BTN_START, pressed, gb);
    }
}

int gb_system_emulate_loop(SDL_Window *win,
                           SDL_Renderer *ren,
                           gb_system_t *gb)
{
    uint32_t clock_speed = CPU_CLOCK_SPEED; // 4.194304 MHz
    const int target_framerate = 60;
    const double target_framerate_delay = (double) 1000.0 / (double) target_framerate;
    bool stop_loop = false;
    bool pause_emulation = false;
    Uint32 ticks = 0, last_ticks;
    double elapsed; // Elasped time in seconds
    double elapsed_ms; // Elapsed time in milliseconds
    double timer_second = 0; // One second timer
    size_t remaining_clocks;
    size_t clocks_per_second = 0;
    uint32_t frames_per_second = 0;
    SDL_Event e;

    while (!stop_loop) {
        last_ticks = ticks;
        ticks = SDL_GetTicks();
        elapsed_ms = ticks - last_ticks;
        elapsed = elapsed_ms / 1000.0;
        timer_second += elapsed;

        if (!pause_emulation) {
            // Calculate how many CPU cycles should be emulated
            // since last frame
            remaining_clocks = elapsed * clock_speed;
            clocks_per_second += remaining_clocks;

            for (; remaining_clocks > 0; --remaining_clocks) {
                if (cpu_cycle(true, gb) < 0) {
                    // Emulation should be stopped
                    stop_loop = true;
                    break;
                }
                ppu_cycle(gb);
                serial_cycle(gb);
            }
        }

        while (SDL_PollEvent(&e)) {
            switch (e.type) {
                case SDL_QUIT:
                    stop_loop = true;
                    break;

                case SDL_KEYDOWN:
                    if (e.key.keysym.sym == SDLK_ESCAPE) {
                        stop_loop = true;
                    } else if (e.key.keysym.sym == SDLK_SPACE) {
                        pause_emulation = !pause_emulation;
                    } else if (e.key.keysym.sym == SDLK_s) {
                        clock_speed = CPU_CLOCK_SPEED * 4;
                    } else if (e.key.keysym.sym == SDLK_d) {
                        debug_enabled ? debug_disable(win) : debug_enable(win);
                    } else {
                        handle_joypad_input(&e, true, gb);
                    }
                    break;

                case SDL_KEYUP:
                    if (e.key.keysym.sym == SDLK_s) {
                        clock_speed = CPU_CLOCK_SPEED;
                    } else {
                        handle_joypad_input(&e, false, gb);
                    }
                    break;

                default: break;
            }
        }

        if (timer_second >= 1.0) {
            if (pause_emulation) {
                update_window_title(
                    win, "GameBoy (paused, %u fps)",
                    (double) clocks_per_second / 1000000.0,
                    (double) clocks_per_second / (double) clock_speed * 100.0,
                    frames_per_second);
            } else {
                update_window_title(
                    win, "GameBoy (%.06f MHz: %.02f%%, %u fps)",
                    (double) clocks_per_second / 1000000.0,
                    (double) clocks_per_second / (double) CPU_CLOCK_SPEED * 100.0,
                    frames_per_second);
            }

            timer_second = 0.0;
            clocks_per_second = 0;
            frames_per_second = 0;
        }

        if (gb->screen.framebuffer_updated || debug_enabled) {
            render_framebuffer(ren, gb);
            gb->screen.framebuffer_updated = false;

            if (debug_enabled)
                render_debug(ren, gb);

            SDL_RenderPresent(ren);
            frames_per_second += 1;
        }

        // Calculate delay before next frame to reach the targeted framerate
        elapsed_ms = SDL_GetTicks() - ticks;
        if (!stop_loop && elapsed_ms < target_framerate_delay) SDL_Delay(target_framerate_delay - elapsed_ms);
    }

    return 0;
}

// GameBoy emulation loop
// Returns < 0 on initialization error
// Returns 0 on success
// Returns > 0 on error during emulation
int gb_system_emulate(gb_system_t *gb, const bool debug)
{
    SDL_Renderer *ren;
    SDL_Window *win;
    SDL_RWops *fontrwo;

    debug_rect.x = SCREEN_WIDTH * SDL_PIXEL_SIZE;
    debug_rect.y = 0;
    debug_rect.w = 0;
    debug_rect.h = SCREEN_HEIGHT * SDL_PIXEL_SIZE;
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS) < 0) {
        fprintf(stderr, "SDL initialization failed: %s\n", SDL_GetError());
        return -1;
    }

    if (TTF_Init() < 0) {
        fprintf(stderr, "SDL_ttf initialization failed: %s\n", TTF_GetError());
        return -1;
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

    if (create_window(&win, &ren) < 0) {
        return -1;
    }

    if (debug)
        debug_enable(win);

    SDL_RenderClear(ren);
    SDL_RenderPresent(ren);

    if (gb->memory.mbc_battery)
        mmu_battery_load(gb);

    printf("Emulating: %s\n", gb->cartridge.title);
    gb_system_emulate_loop(win, ren, gb);
    printf("Emulation stopped\n");

    if (gb->memory.mbc_battery)
        mmu_battery_save(gb);

    TTF_CloseFont(font);
    TTF_Quit();
    SDL_DestroyRenderer(ren);
    SDL_DestroyWindow(win);
    SDL_Quit();
    return 0;
}