/*
emulator_utils.c
Emulator utilities

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

#include "moderndos8x16_ttf.h"
#include <SDL.h>
#include <SDL_ttf.h>

// Render text using *font
void render_text(TTF_Font *font,
                 SDL_Renderer *ren,
                 int x,
                 int y,
                 const char *format,
                 ...)
{
    SDL_Surface *surface;
    SDL_Texture *texture;
    SDL_Rect rect;
    char text[256];
    va_list ap;

    va_start(ap, format);
    vsnprintf(text, sizeof(text), format, ap);
    va_end(ap);

    if (!(surface = TTF_RenderText_Solid(font, text, (SDL_Color) {0, 0, 0, 255}))) {
        fprintf(stderr, "TTF_RenderText_Solid: %s\n", TTF_GetError());
        return;
    }
    if (!(texture = SDL_CreateTextureFromSurface(ren, surface))) {
        fprintf(stderr, "SDL_CreateTextureFromSurface: %s\n", SDL_GetError());
        SDL_FreeSurface(surface);
        return;
    }

    rect.x = x;
    rect.y = y;
    TTF_SizeText(font, text, &rect.w, &rect.h);
    SDL_RenderCopy(ren, texture, NULL, &rect);
    SDL_FreeSurface(surface);
    SDL_DestroyTexture(texture);
}

// Load default TTF font
// Returns NULL on error
TTF_Font *load_default_font(void)
{
    SDL_RWops *rwo;
    TTF_Font *font;

    if (!(rwo = SDL_RWFromConstMem(ModernDOS8x16_ttf, ModernDOS8x16_ttf_len))) {
        fprintf(stderr, "SDL_RWFromConstMem: %s\n", SDL_GetError());
        return NULL;
    }
    if (!(font = TTF_OpenFontRW(rwo, 1, 16))) {
        fprintf(stderr, "TTF_OpenFont: %s\n", TTF_GetError());
        return NULL;
    }
    return font;
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