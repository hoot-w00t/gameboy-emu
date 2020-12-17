/*
mmu_view.c
Inspect memory and MBC states

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
#include "emulator_utils.h"
#include "emulator_events.h"
#include "cartridge.h"
#include "mmu/mmu.h"
#include "mmu/rambanks.h"

#define W_WIDTH  (500)
#define W_HEIGHT (64)

static TTF_Font     *mmu_view_font   = NULL;
static int           line_height     = 0;

static SDL_Window   *mmu_view_win    = NULL;
static uint32_t      mmu_view_win_id = 0;
static SDL_Renderer *mmu_view_ren    = NULL;

// Close MMU view window
void mmu_view_close(void)
{
    if (mmu_view_win) {
        SDL_DestroyRenderer(mmu_view_ren);
        SDL_DestroyWindow(mmu_view_win);
        TTF_CloseFont(mmu_view_font);
        mmu_view_ren = NULL;
        mmu_view_win = NULL;
        emu_windows_clear(EMU_WINDOWS_MMU_VIEW);
    }
}

// Handle MMU view event
void mmu_view_event(SDL_Event *e, __attribute__((unused)) gb_system_t *gb)
{
    switch (e->type) {
        case SDL_WINDOWEVENT:
            if (e->window.event == SDL_WINDOWEVENT_CLOSE)
                mmu_view_close();
            break;

        default: break;
    }
}

// Open MMU view window
void mmu_view_open(void)
{
    if (!mmu_view_win) {
        SDL_CreateWindowAndRenderer(W_WIDTH, W_HEIGHT, 0, &mmu_view_win, &mmu_view_ren);
        mmu_view_font = load_default_font();

        if (!mmu_view_win || !mmu_view_ren || !mmu_view_font) {
            fprintf(stderr, "mmu_view_open: %s\n", SDL_GetError());
            if (mmu_view_ren) SDL_DestroyRenderer(mmu_view_ren);
            if (mmu_view_win) SDL_DestroyWindow(mmu_view_win);
            if (mmu_view_font) TTF_CloseFont(mmu_view_font);
            mmu_view_ren = NULL;
            mmu_view_win = NULL;
            mmu_view_font = NULL;
            emu_windows_clear(EMU_WINDOWS_MMU_VIEW);
            return;
        }
        emu_windows_set(EMU_WINDOWS_MMU_VIEW, mmu_view_win, &mmu_view_event);
        line_height = TTF_FontHeight(mmu_view_font);
        mmu_view_win_id = SDL_GetWindowID(mmu_view_win);
        update_window_title(mmu_view_win, "GameBoy MMU");
    }
}

// Render the MMU view window
void mmu_view_render(gb_system_t *gb)
{
    const int coll = 3;
    char ram_access[32];

    SDL_SetRenderDrawColor(mmu_view_ren, 255, 204, 153, 255);
    SDL_RenderClear(mmu_view_ren);

    if (gb->cartridge.mbc_type == 0x0) {
        render_text(mmu_view_font, mmu_view_ren, coll, 0, "No MBC (ROM Only)");
    } else {
        render_text(mmu_view_font, mmu_view_ren, coll, 0, "%s ($%02X)",
            cartridge_mbc_type(&gb->cartridge),
            gb->cartridge.mbc_type);
    }

    render_text(mmu_view_font, mmu_view_ren, coll, line_height * 1,
        "$0000-$3FFF: ROM bank #%-4u $%-X",
        gb->memory.rom.bank_0,
        gb->memory.rom.bank_0);
    render_text(mmu_view_font, mmu_view_ren, coll, line_height * 2,
        "$4000-$7FFF: ROM bank #%-4u $%-4X (%u available)",
        gb->memory.rom.bank_n,
        gb->memory.rom.bank_n,
        gb->memory.rom.banks_nb);

    if (rambank_exists(&gb->memory.ram)) {
        if (gb->memory.ram.can_read && gb->memory.ram.can_write) {
            snprintf(ram_access, sizeof(ram_access), "read/write");
        } else if (!gb->memory.ram.can_read && gb->memory.ram.can_write) {
            snprintf(ram_access, sizeof(ram_access), "write-only");
        } else if (gb->memory.ram.can_read && !gb->memory.ram.can_write) {
            snprintf(ram_access, sizeof(ram_access), "read-only");
        } else {
            snprintf(ram_access, sizeof(ram_access), "disabled");
        }

        render_text(mmu_view_font, mmu_view_ren, coll, line_height * 3,
            "$A000-$BFFF: RAM bank #%-4u $%-4X (%u available) (%s)",
            gb->memory.ram.bank,
            gb->memory.ram.bank,
            gb->memory.ram.banks_nb,
            ram_access);
    } else {
        render_text(mmu_view_font, mmu_view_ren, coll, line_height * 3,
            "$A000-$BFFF: No external RAM banks");
    }

    SDL_RenderPresent(mmu_view_ren);
}