/*
bit.c
BIT CB opcodes

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

static inline void cpu_test_bit(const byte_t target, const byte_t bit, gb_system_t *gb)
{
    if ((target & (1 << bit))) reg_flag_clear(FLAG_Z, gb); else reg_flag_set(FLAG_Z, gb);
    reg_flag_clear(FLAG_N, gb);
    reg_flag_set(FLAG_H, gb);
}

// BIT b,n opcodes
int opcode_cb_bit(const opcode_t *opcode, gb_system_t *gb)
{
    byte_t bit;
    byte_t op_lo = 0x40;

    for (bit = 0; bit < 8; ++bit, op_lo += 8) {
        if (opcode->opcode >= op_lo && opcode->opcode <= op_lo + 7)
            break;
    }

    if (bit > 7) return OPCODE_ILLEGAL;
    switch (opcode->opcode - op_lo) {
        case 0: cpu_test_bit(reg_readb(REG_B, gb), bit, gb); break;
        case 1: cpu_test_bit(reg_readb(REG_C, gb), bit, gb); break;
        case 2: cpu_test_bit(reg_readb(REG_D, gb), bit, gb); break;
        case 3: cpu_test_bit(reg_readb(REG_E, gb), bit, gb); break;
        case 4: cpu_test_bit(reg_readb(REG_H, gb), bit, gb); break;
        case 5: cpu_test_bit(reg_readb(REG_L, gb), bit, gb); break;
        case 6: cpu_test_bit(mmu_readb(reg_read_u16(REG_HL, gb), gb), bit, gb); break;
        case 7: cpu_test_bit(reg_readb(REG_A, gb), bit, gb); break;
        default: return OPCODE_ILLEGAL;
    }

    return opcode->cycles_true;
}