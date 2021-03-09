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
    byte_t reg = REG_A;
    byte_t value;

    if (op_lo == 6) {
        value = mmu_readb(reg_read_u16(REG_HL, gb), gb);
    } else {
        if (op_lo <= 5)
            reg = REG_B + op_lo;
        value = reg_readb(reg, gb);
    }
    value |= (1 << bit);
    if (op_lo == 6) {
        mmu_writeb(reg_read_u16(REG_HL, gb), value, gb);
    } else {
        reg_writeb(reg, value, gb);
    }
    return opcode->cycles_true;
}