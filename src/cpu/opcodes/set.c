/*
set.c
SET CB opcodes

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

// SET b,n opcodes
int opcode_cb_set(const opcode_t *opcode, gb_system_t *gb)
{
    const byte_t bit = (opcode->opcode - 0xC0) / 8;
    const byte_t op_lo = opcode->opcode % 8;
    uint16_t addr;

    switch (op_lo) {
        case 0: gb->regs.b |= (1 << bit); return opcode->cycles_true;
        case 1: gb->regs.c |= (1 << bit); return opcode->cycles_true;
        case 2: gb->regs.d |= (1 << bit); return opcode->cycles_true;
        case 3: gb->regs.e |= (1 << bit); return opcode->cycles_true;
        case 4: gb->regs.h |= (1 << bit); return opcode->cycles_true;
        case 5: gb->regs.l |= (1 << bit); return opcode->cycles_true;
        case 6:
            addr = reg_read_hl(gb);
            mmu_writeb(addr, (mmu_readb(addr, gb) | (1 << bit)), gb);
            return opcode->cycles_true;
        case 7: gb->regs.a |= (1 << bit); return opcode->cycles_true;
        default: return OPCODE_ILLEGAL;
    }
}