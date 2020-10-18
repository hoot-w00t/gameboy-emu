/*
res.c
RES CB opcodes

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

// RES b,n opcodes
int opcode_cb_res(const opcode_t *opcode, gb_system_t *gb)
{
    byte_t bit;
    byte_t value;
    byte_t op_lo = 0x80;

    for (bit = 0; bit < 8; ++bit, op_lo += 8) {
        if (opcode->opcode >= op_lo && opcode->opcode <= op_lo + 7)
            break;
    }

    if (bit > 7) return OPCODE_ILLEGAL;
    switch (opcode->opcode - op_lo) {
        case 0: value = reg_readb(REG_B, gb); break;
        case 1: value = reg_readb(REG_C, gb); break;
        case 2: value = reg_readb(REG_D, gb); break;
        case 3: value = reg_readb(REG_E, gb); break;
        case 4: value = reg_readb(REG_H, gb); break;
        case 5: value = reg_readb(REG_L, gb); break;
        case 6: value = mmu_readb(reg_read_u16(REG_HL, gb), gb); break;
        case 7: value = reg_readb(REG_A, gb); break;
        default: return OPCODE_ILLEGAL;
    }

    if ((value & (1 << bit))) value ^= (1 << bit);

    switch (opcode->opcode - op_lo) {
        case 0: reg_writeb(REG_B, value, gb); break;
        case 1: reg_writeb(REG_C, value, gb); break;
        case 2: reg_writeb(REG_D, value, gb); break;
        case 3: reg_writeb(REG_E, value, gb); break;
        case 4: reg_writeb(REG_H, value, gb); break;
        case 5: reg_writeb(REG_L, value, gb); break;
        case 6: mmu_writeb(reg_read_u16(REG_HL, gb), value, gb); break;
        case 7: reg_writeb(REG_A, value, gb); break;
        default: return OPCODE_ILLEGAL;
    }

    return opcode->cycles_true;
}