/*
swap.c
SWAP opcodes

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
#include "cpu/registers.h"
#include "mmu/mmu.h"

// SWAP r
int opcode_cb_swap_r(const opcode_t *opcode, gb_system_t *gb)
{
    byte_t *reg;

    switch (opcode->opcode) {
        case 0x37: reg = &gb->regs.a; break;
        case 0x30: reg = &gb->regs.b; break;
        case 0x31: reg = &gb->regs.c; break;
        case 0x32: reg = &gb->regs.d; break;
        case 0x33: reg = &gb->regs.e; break;
        case 0x34: reg = &gb->regs.h; break;
        case 0x35: reg = &gb->regs.l; break;
        default: return OPCODE_ILLEGAL;
    }
    *reg = ((*reg) >> 4) | ((*reg) << 4);
    gb->regs.f.flags.z = (*reg) == 0;
    gb->regs.f.flags.n = 0;
    gb->regs.f.flags.h = 0;
    gb->regs.f.flags.c = 0;
    return opcode->cycles_true;
}

// SWAP (HL)
int opcode_cb_swap_n(const opcode_t *opcode, gb_system_t *gb)
{
    const uint16_t addr = reg_read_hl(gb);
    const byte_t value = mmu_readb(addr, gb);
    const byte_t result = (value >> 4) | (value << 4);

    mmu_writeb(addr, result, gb);
    gb->regs.f.flags.z = result == 0;
    gb->regs.f.flags.n = 0;
    gb->regs.f.flags.h = 0;
    gb->regs.f.flags.c = 0;
    return opcode->cycles_true;
}