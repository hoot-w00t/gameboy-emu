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
#include "serial.h"
#include <SDL.h>
#include <SDL_ttf.h>
#include <SDL_net.h>

// Initialize the required parts of the SDL, exit on error
void initialize_sdl(void)
{
    if (TTF_Init() < 0) {
        fprintf(stderr, "SDL_ttf initialization failed: %s\n", TTF_GetError());
        exit(EXIT_FAILURE);
    }
    atexit(TTF_Quit);

    if (SDLNet_Init() < 0) {
        fprintf(stderr, "SDL_net initialization failed: %s\n", SDLNet_GetError());
        exit(EXIT_FAILURE);
    }
    atexit(SDLNet_Quit);

    if (serial_init() < 0) {
        exit(EXIT_FAILURE);
    }
    atexit(serial_quit);

    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS) < 0) {
        fprintf(stderr, "SDL initialization failed: %s\n", SDL_GetError());
        exit(EXIT_FAILURE);
    }
    atexit(SDL_Quit);
}

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

// Render outlined text using *font
void render_text_outline(TTF_Font *font,
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

    rect.x = x;
    rect.y = y;
    TTF_SetFontOutline(font, 1);
    TTF_SizeText(font, text, &rect.w, &rect.h);
    if (!(surface = TTF_RenderText_Solid(font, text, (SDL_Color) {0, 0, 0, 255}))) {
        fprintf(stderr, "TTF_RenderText_Solid: %s\n", TTF_GetError());
        return;
    }
    if (!(texture = SDL_CreateTextureFromSurface(ren, surface))) {
        fprintf(stderr, "SDL_CreateTextureFromSurface: %s\n", SDL_GetError());
        SDL_FreeSurface(surface);
        return;
    }
    SDL_RenderCopy(ren, texture, NULL, &rect);
    SDL_FreeSurface(surface);
    SDL_DestroyTexture(texture);

    TTF_SetFontOutline(font, 0);
    TTF_SizeText(font, text, &rect.w, &rect.h);
    if (!(surface = TTF_RenderText_Solid(font, text, (SDL_Color) {220, 220, 220, 255}))) {
        fprintf(stderr, "TTF_RenderText_Solid: %s\n", TTF_GetError());
        return;
    }
    if (!(texture = SDL_CreateTextureFromSurface(ren, surface))) {
        fprintf(stderr, "SDL_CreateTextureFromSurface: %s\n", SDL_GetError());
        SDL_FreeSurface(surface);
        return;
    }
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

// Await file drop on the window
// Returns the file path or NULL on error
char *ask_for_file_drop(void)
{
    const int w_width = 300;
    const int w_height = 300;
    const char text[] = "Drag and drop GameBoy ROM here";
    SDL_Window *win;
    SDL_Renderer *ren;
    TTF_Font *font;
    SDL_Surface *surface;
    SDL_Texture *texture;
    SDL_Rect rect;
    SDL_Event e;
    char *filename = NULL;

    font = load_default_font();
    SDL_CreateWindowAndRenderer(w_width, w_height, 0, &win, &ren);
    surface = TTF_RenderText_Solid(font, text, (SDL_Color) {255, 255, 255, 255});
    texture = SDL_CreateTextureFromSurface(ren, surface);
    if (!win || !ren || !font || !surface || !texture) {
        fprintf(stderr, "ask_for_file_drop: %s\n", SDL_GetError());
        exit(EXIT_FAILURE);
    }

    SDL_SetRenderDrawColor(ren, 0, 0, 0, 255);
    SDL_RenderClear(ren);
    TTF_SizeText(font, text, &rect.w, &rect.h);
    rect.x = (w_width / 2) - (rect.w / 2);
    rect.y = (w_height / 2) - (rect.h / 2);
    SDL_RenderCopy(ren, texture, NULL, &rect);
    SDL_RenderPresent(ren);

    while (1) {
        while (SDL_PollEvent(&e)) {
            switch (e.type) {
                case SDL_QUIT: goto drop_end;
                case SDL_KEYDOWN:
                    if (e.key.keysym.scancode == SDL_SCANCODE_ESCAPE)
                        goto drop_end;
                    break;
                case SDL_WINDOWEVENT:
                    if (e.window.event == SDL_WINDOWEVENT_CLOSE)
                        goto drop_end;
                    break;
                case SDL_DROPFILE:
                    filename = SDL_strdup(e.drop.file);
                    goto drop_end;
                default: break;
            }
        }
        SDL_Delay(50);
    }

drop_end:
    SDL_DestroyRenderer(ren);
    SDL_DestroyWindow(win);
    SDL_FreeSurface(surface);
    SDL_DestroyTexture(texture);
    TTF_CloseFont(font);
    return filename;
}