/*
dec.c
DEC opcodes

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

// Decrement byte
// Affected flags
//     Z if result == 0
//     N set
//     H if no borrow from bit 4
byte_t cpu_decb(const byte_t target, gb_system_t *gb)
{
    byte_t result = target - 1;

    if (result == 0) {
        reg_flag_set(FLAG_Z, gb);
    } else {
        reg_flag_clear(FLAG_Z, gb);
    }
    reg_flag_set(FLAG_N, gb);
    if ((target & 0xF) == 0) {
        reg_flag_set(FLAG_H, gb);
    } else {
        reg_flag_clear(FLAG_H, gb);
    }

    return result;
}

// DEC n opcodes (registers only)
int opcode_dec_n_r(const opcode_t *opcode, gb_system_t *gb)
{
    byte_t reg;
    byte_t result;

    switch (opcode->opcode) {
        case 0x3D: reg = REG_A; break;
        case 0x05: reg = REG_B; break;
        case 0x0D: reg = REG_C; break;
        case 0x15: reg = REG_D; break;
        case 0x1D: reg = REG_E; break;
        case 0x25: reg = REG_H; break;
        case 0x2D: reg = REG_L; break;
        default: return OPCODE_ILLEGAL;
    }

    result = cpu_decb(reg_readb(reg, gb), gb);
    reg_writeb(reg, result, gb);

    return opcode->cycles_true;
}

// DEC (HL)
int opcode_dec_n_hl(const opcode_t *opcode, gb_system_t *gb)
{
    uint16_t addr = reg_read_u16(REG_HL, gb);
    byte_t result = cpu_decb(mmu_readb(addr, gb), gb);

    mmu_writeb(addr, result, gb);
    return opcode->cycles_true;
}

// DEC nn opcodes
int opcode_dec_nn(const opcode_t *opcode, gb_system_t *gb)
{
    switch (opcode->opcode) {
        case 0x0B: reg_write_u16(REG_BC, reg_read_u16(REG_BC, gb) - 1, gb); break;
        case 0x1B: reg_write_u16(REG_DE, reg_read_u16(REG_DE, gb) - 1, gb); break;
        case 0x2B: reg_write_u16(REG_HL, reg_read_u16(REG_HL, gb) - 1, gb); break;
        case 0x3B: gb->sp -= 1; break;
        default: return OPCODE_ILLEGAL;
    }
    return opcode->cycles_true;
}