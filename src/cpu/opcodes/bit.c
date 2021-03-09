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
    const byte_t bit = (opcode->opcode - 0x40) / 8;
    const byte_t op_lo = opcode->opcode % 8;

    if (op_lo == 6) {
        cpu_test_bit(mmu_read_u16(reg_read_u16(REG_HL, gb), gb), bit, gb);
    } else {
        if (op_lo <= 5) {
            cpu_test_bit(reg_readb(op_lo + REG_B, gb), bit, gb);
        } else {
            cpu_test_bit(reg_readb(REG_A, gb), bit, gb);
        }
    }
    return opcode->cycles_true;
}