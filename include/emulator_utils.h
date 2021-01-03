/*
emulator_utils.h
Function prototypes for emulator_utils.c

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

#include <SDL.h>
#include <SDL_ttf.h>

#ifndef _EMULATOR_UTILS_H
#define _EMULATOR_UTILS_H

void initialize_sdl(void);
void render_text(TTF_Font *font,
                 SDL_Renderer *ren,
                 int x,
                 int y,
                 const char *format,
                 ...);
TTF_Font *load_default_font(void);
void update_window_title(SDL_Window *win, const char *format, ...);
char *ask_for_file_drop(void);

#endif