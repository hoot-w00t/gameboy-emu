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
#include "ppu/ppu.h"
#include "joypad.h"
#include <stdio.h>
#include <SDL.h>

// TODO: Use a proper pixel size variable
#define SDL_PIXEL_SIZE (4)

// create SDL window
int create_window(SDL_Window **win, SDL_Renderer **ren)
{
    SDL_CreateWindowAndRenderer(
        SCREEN_WIDTH * SDL_PIXEL_SIZE,
        SCREEN_HEIGHT * SDL_PIXEL_SIZE,
        0,
        win,
        ren);

    if (*win == NULL || *ren == NULL) {
        fprintf(stderr, "Unable to create SDL window: %s\n", SDL_GetError());
        return -1;
    }

    return 0;
}

// update SDL window size
void update_window(SDL_Window *win)
{
    SDL_SetWindowSize(
        win,
        SCREEN_WIDTH * SDL_PIXEL_SIZE,
        SCREEN_HEIGHT * SDL_PIXEL_SIZE);
}

// update SDL window title
void update_window_title(SDL_Window *win, const char *format, ...)
{
    char title[256];
    va_list ap;

    va_start(ap, format);
    vsnprintf(
        title,
        sizeof(title),
        format,
        ap);
    va_end(ap);

    SDL_SetWindowTitle(win, title);
}

// draw a scaled pixel
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

// render the gb->screen.framebuffer
void render_framebuffer(SDL_Renderer *ren, gb_system_t *gb)
{
    SDL_SetRenderDrawColor(ren, 0, 0, 0, 255);
    SDL_RenderClear(ren);

    for (uint16_t y = 0; y < SCREEN_HEIGHT; ++y) {
        for (uint16_t x = 0; x < SCREEN_WIDTH; ++x) {
            draw_pixel(ren, y, x, SDL_PIXEL_SIZE, &gb->screen.framebuffer[y][x]);
        }
    }

    SDL_RenderPresent(ren);
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

int gb_system_emulate_loop(SDL_Window *win, SDL_Renderer *ren, gb_system_t *gb)
{
    const uint32_t cpu_clock_speed = 4194304; // 4.194304 MHz
    const int target_framerate = 60;
    const double target_framerate_delay = (double) 1000.0 / (double) target_framerate;
    bool stop_loop = false;
    bool pause_emulation = false;
    Uint32 ticks = 0, last_ticks;
    double elapsed; // Elasped time in seconds
    double elapsed_ms; // Elapsed time in milliseconds
    double timer_second = 0; // One second timer
    size_t remaining_cpu_cycles;
    size_t cpu_cycles_per_second = 0;
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
            remaining_cpu_cycles = elapsed * cpu_clock_speed;
            cpu_cycles_per_second += remaining_cpu_cycles;

            for (; remaining_cpu_cycles > 0; --remaining_cpu_cycles) {
                if (cpu_cycle(true, gb) < 0) {
                    // Emulation should be stopped
                    stop_loop = true;
                    break;
                }
                ppu_cycle(gb);
            }

        }

        while (SDL_PollEvent(&e)) {
            switch (e.type) {
                case SDL_QUIT: stop_loop = true; break;
                case SDL_KEYDOWN:
                    if (e.key.keysym.sym == SDLK_ESCAPE) stop_loop = true;
                    else if (e.key.keysym.sym == SDLK_SPACE) pause_emulation = !pause_emulation;
                    else handle_joypad_input(&e, true, gb);
                    break;
                case SDL_KEYUP: handle_joypad_input(&e, false, gb); break;
                default: break;
            }
        }

        if (timer_second >= 1.0) {
            if (pause_emulation) {
                update_window_title(
                    win,
                    "GameBoy (paused, %u fps)",
                    (double) cpu_cycles_per_second / 1000000.0,
                    (double) cpu_cycles_per_second / (double) cpu_clock_speed * 100.0,
                    frames_per_second);
            } else {
                update_window_title(
                    win,
                    "GameBoy (%.06f MHz: %.02f%%, %u fps)",
                    (double) cpu_cycles_per_second / 1000000.0,
                    (double) cpu_cycles_per_second / (double) cpu_clock_speed * 100.0,
                    frames_per_second);
            }

            timer_second = 0.0;
            cpu_cycles_per_second = 0;
            frames_per_second = 0;
        }

        if (gb->screen.framebuffer_updated) {
            gb->screen.framebuffer_updated = false;
            render_framebuffer(ren, gb);
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
int gb_system_emulate(gb_system_t *gb)
{
    SDL_Renderer *ren;
    SDL_Window *win;

    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS) < 0) {
        fprintf(stderr, "SDL initialization failed: %s\n", SDL_GetError());
        return -1;
    }

    if (create_window(&win, &ren) < 0) {
        SDL_Quit();
        return -1;
    }

    SDL_RenderClear(ren);
    SDL_RenderPresent(ren);

    printf("Emulating: %s\n", gb->cartridge.title);
    gb_system_emulate_loop(win, ren, gb);
    printf("Emulation stopped\n");

    SDL_DestroyRenderer(ren);
    SDL_DestroyWindow(win);
    SDL_Quit();
    return 0;
}