/*
emulator_events.h
Function prototypes and structures for emulator_events.c

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

#include "gameboy.h"
#include <SDL.h>

#ifndef _EMULATOR_EVENTS_H
#define _EMULATOR_EVENTS_H

typedef void (*handle_event_t)(SDL_Event *e, gb_system_t *gb);

struct keymap {
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
    SDL_Scancode emu_exit;
    SDL_Scancode emu_zoom_in;
    SDL_Scancode emu_zoom_out;
    SDL_Scancode emu_vol_up;
    SDL_Scancode emu_vol_down;
    SDL_Scancode emu_cpu_view;
};

struct emu_windows {
    SDL_Window *win;
    Uint32 id;
    handle_event_t handle_event;
};

#define EMU_WINDOWS_SIZE     (2)

enum emu_windows_index {
    EMU_WINDOWS_LCD      = 0,
    EMU_WINDOWS_CPU_VIEW = 1
};

#endif

#ifndef _EMULATOR_EVENTS_EXTERNS
#define _EMULATOR_EVENTS_EXTERNS

extern struct keymap emu_keymap;
extern struct emu_windows emu_windows[EMU_WINDOWS_SIZE];

static inline void emu_windows_set(const enum emu_windows_index index,
                                   SDL_Window *win,
                                   handle_event_t handle_event)
{
    emu_windows[index].win = win;
    emu_windows[index].id = SDL_GetWindowID(win);
    emu_windows[index].handle_event = handle_event;
}

static inline void emu_windows_clear(const enum emu_windows_index index)
{
    emu_windows[index].win = NULL;
    emu_windows[index].id = 0;
    emu_windows[index].handle_event = NULL;
}

#endif