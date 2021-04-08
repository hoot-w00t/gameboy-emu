/*
cpu_view.c
Inspect CPU state while the emulator is running

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
#include "cpu/registers.h"
#include "cpu/opcodes.h"
#include "mmu/mmu.h"

#define W_WIDTH  (400)
#define W_HEIGHT (220)

static TTF_Font     *cpu_view_font   = NULL;
static int           line_height     = 0;

static SDL_Window   *cpu_view_win    = NULL;
static uint32_t      cpu_view_win_id = 0;
static SDL_Renderer *cpu_view_ren    = NULL;

// Close CPU debugger window
void cpu_view_close(void)
{
    if (cpu_view_win) {
        SDL_DestroyRenderer(cpu_view_ren);
        SDL_DestroyWindow(cpu_view_win);
        TTF_CloseFont(cpu_view_font);
        cpu_view_ren = NULL;
        cpu_view_win = NULL;
        emu_windows_clear(EMU_WINDOWS_CPU_VIEW);
    }
}

// Handle CPU view event
void cpu_view_event(SDL_Event *e, __attribute__((unused)) gb_system_t *gb)
{
    switch (e->type) {
        case SDL_WINDOWEVENT:
            if (e->window.event == SDL_WINDOWEVENT_CLOSE)
                cpu_view_close();
            break;

        default: break;
    }
}

// Open debugger window
void cpu_view_open(void)
{
    if (!cpu_view_win) {
        SDL_CreateWindowAndRenderer(W_WIDTH, W_HEIGHT, 0, &cpu_view_win, &cpu_view_ren);
        cpu_view_font = load_default_font();

        if (!cpu_view_win || !cpu_view_ren || !cpu_view_font) {
            fprintf(stderr, "cpu_view_open: %s\n", SDL_GetError());
            if (cpu_view_ren) SDL_DestroyRenderer(cpu_view_ren);
            if (cpu_view_win) SDL_DestroyWindow(cpu_view_win);
            if (cpu_view_font) TTF_CloseFont(cpu_view_font);
            cpu_view_ren = NULL;
            cpu_view_win = NULL;
            cpu_view_font = NULL;
            emu_windows_clear(EMU_WINDOWS_CPU_VIEW);
            return;
        }
        emu_windows_set(EMU_WINDOWS_CPU_VIEW, cpu_view_win, &cpu_view_event);
        line_height = TTF_FontHeight(cpu_view_font);
        cpu_view_win_id = SDL_GetWindowID(cpu_view_win);
        update_window_title(cpu_view_win, "GameBoy CPU");
    }
}

// Render the CPU view window
void cpu_view_render(gb_system_t *gb)
{
    const int coll = 5;
    const int colr = coll + ((W_WIDTH - 5) / 2);
    const int w4 = (W_WIDTH - 5) / 4;
    const int w3 = (W_WIDTH - 5) / 3;
    const opcode_t *opcode;
    byte_t pc_byte;

    if ((pc_byte = mmu_readb_nolog(gb->pc, gb)) == 0xCB) {
        opcode = opcode_cb_identify(mmu_readb_nolog(gb->pc + 1, gb));
    } else {
        opcode = opcode_identify(pc_byte);
    }

    SDL_SetRenderDrawColor(cpu_view_ren, 204, 255, 204, 255);
    SDL_RenderClear(cpu_view_ren);
    render_text(cpu_view_font, cpu_view_ren, coll, line_height * 0, "Cycle #%lu", gb->cycle_nb);
    render_text(cpu_view_font, cpu_view_ren, coll, line_height * 1, "PC: $%04X", gb->pc);
    render_text(cpu_view_font, cpu_view_ren, colr, line_height * 1, "SP: $%04X", gb->sp);
    render_text(cpu_view_font, cpu_view_ren, coll, line_height * 2, "A: $%02X", gb->regs.a);
    render_text(cpu_view_font, cpu_view_ren, colr, line_height * 2, "F: $%02X", gb->regs.f);
    render_text(cpu_view_font, cpu_view_ren, coll, line_height * 3, "B: $%02X", gb->regs.b);
    render_text(cpu_view_font, cpu_view_ren, colr, line_height * 3, "C: $%02X", gb->regs.c);
    render_text(cpu_view_font, cpu_view_ren, coll, line_height * 4, "D: $%02X", gb->regs.d);
    render_text(cpu_view_font, cpu_view_ren, colr, line_height * 4, "E: $%02X", gb->regs.e);
    render_text(cpu_view_font, cpu_view_ren, coll, line_height * 5, "H: $%02X", gb->regs.h);
    render_text(cpu_view_font, cpu_view_ren, colr, line_height * 5, "L: $%02X", gb->regs.l);
    render_text(cpu_view_font, cpu_view_ren, coll,          line_height * 6, "Z: %u", gb->regs.f.flags.z);
    render_text(cpu_view_font, cpu_view_ren, coll + w4,     line_height * 6, "H: %u", gb->regs.f.flags.h);
    render_text(cpu_view_font, cpu_view_ren, coll + w4 * 2, line_height * 6, "N: %u", gb->regs.f.flags.n);
    render_text(cpu_view_font, cpu_view_ren, coll + w4 * 3, line_height * 6, "C: %u", gb->regs.f.flags.c);
    render_text(cpu_view_font, cpu_view_ren, coll,          line_height * 7, "IME: %u", gb->interrupts.ime);
    render_text(cpu_view_font, cpu_view_ren, coll + w3,     line_height * 7, "IE: $%02X", gb->interrupts.ie_reg);
    render_text(cpu_view_font, cpu_view_ren, coll + w3 * 2, line_height * 7, "IF: $%02X", gb->interrupts.if_reg);
    if (gb->halt) render_text(cpu_view_font, cpu_view_ren, coll, line_height * 8, "Halted");
    if (gb->stop) render_text(cpu_view_font, cpu_view_ren, colr, line_height * 8, "Stopped");
    render_text(cpu_view_font, cpu_view_ren, coll, line_height * 10, "Next instruction:");
    if (opcode) {
        render_text(cpu_view_font, cpu_view_ren, coll, line_height * 11, opcode->mnemonic);
        render_text(cpu_view_font, cpu_view_ren, coll, line_height * 12, opcode->comment);
    } else {
        render_text(cpu_view_font, cpu_view_ren, coll, line_height * 11, "Illegal ($%02X)", pc_byte);
    }
    SDL_RenderPresent(cpu_view_ren);
}