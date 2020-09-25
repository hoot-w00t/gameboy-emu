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
#include <stdio.h>
#include <SDL.h>

int gb_system_emulate_loop(gb_system_t *gb)
{
    const uint32_t cpu_clock_speed = 4194304; // 4.194304 MHz
    const int target_framerate = 60;
    const int target_framerate_delay = 1000 / target_framerate;
    bool stop_loop = false;
    bool pause_emulation = false;
    Uint32 ticks = 0, last_ticks;
    double elapsed; // Elasped time in seconds
    size_t remaining_cpu_cycles;

    while (!stop_loop) {
        last_ticks = ticks;
        ticks = SDL_GetTicks();
        elapsed = (ticks - last_ticks) / 1000.0;

        if (!pause_emulation) {
            // Calculate how many CPU cycles should be emulated
            // since last frame
            remaining_cpu_cycles = elapsed * cpu_clock_speed;

            for (; remaining_cpu_cycles > 0; --remaining_cpu_cycles) {
                if (cpu_cycle(true, gb) < 0) {
                    // Emulation should be stopped
                    stop_loop = true;
                    break;
                }
            }

        }

        // Don't delay if emulation is stopped
        if (!stop_loop) SDL_Delay(target_framerate_delay);
    }

    return 0;
}

// GameBoy emulation loop
// Returns < 0 on initialization error
// Returns 0 on success
// Returns > 0 on error during emulation
int gb_system_emulate(gb_system_t *gb)
{
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS) < 0) {
        fprintf(stderr, "SDL initialization failed: %s\n", SDL_GetError());
        return -1;
    }

    printf("Emulating: %s\n", gb->cartridge.title);
    gb_system_emulate_loop(gb);
    printf("Emulation stopped\n");

    SDL_Quit();
    return 0;
}