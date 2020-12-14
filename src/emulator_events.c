/*
emulator_events.c
GameBoy emulator globals for event handling

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

#define _EMULATOR_EVENTS_EXTERNS

#include "xalloc.h"
#include "emulator_events.h"
#include <stdlib.h>
#include <stddef.h>

struct keymap emu_keymap = {
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
    .emu_exit      = SDL_SCANCODE_ESCAPE,
    .emu_zoom_in   = SDL_SCANCODE_9,
    .emu_zoom_out  = SDL_SCANCODE_8,
    .emu_vol_up    = SDL_SCANCODE_7,
    .emu_vol_down  = SDL_SCANCODE_6,
    .emu_cpu_view  = SDL_SCANCODE_1
};

struct emu_windows emu_windows[EMU_WINDOWS_SIZE];