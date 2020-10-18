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
    byte_t reg;
    byte_t value;
    byte_t result;

    switch (opcode->opcode) {
        case 0x37: reg = REG_A; break;
        case 0x30: reg = REG_B; break;
        case 0x31: reg = REG_C; break;
        case 0x32: reg = REG_D; break;
        case 0x33: reg = REG_E; break;
        case 0x34: reg = REG_H; break;
        case 0x35: reg = REG_L; break;
        default: return OPCODE_ILLEGAL;
    }

    value = reg_readb(reg, gb);
    result = (value >> 4) | (value << 4);
    reg_writeb(reg, result, gb);

    if (result == 0) reg_flag_set(FLAG_Z, gb); else reg_flag_clear(FLAG_Z, gb);
    reg_flag_clear(FLAG_N, gb);
    reg_flag_clear(FLAG_H, gb);
    reg_flag_clear(FLAG_C, gb);

    return opcode->cycles_true;
}

// SWAP (HL)
int opcode_cb_swap_n(const opcode_t *opcode, gb_system_t *gb)
{
    uint16_t addr = reg_read_u16(REG_HL, gb);
    byte_t value = mmu_readb(addr, gb);
    byte_t result = (value >> 4) | (value << 4);

    mmu_writeb(addr, result, gb);

    if (result == 0) reg_flag_set(FLAG_Z, gb); else reg_flag_clear(FLAG_Z, gb);
    reg_flag_clear(FLAG_N, gb);
    reg_flag_clear(FLAG_H, gb);
    reg_flag_clear(FLAG_C, gb);

    return opcode->cycles_true;
}