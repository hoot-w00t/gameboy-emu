/*
inc.c
INC opcodes

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

// Increment byte
// Affected flags
//     Z set if result == 0
//     N reset
//     H carry from bit 3
byte_t cpu_incb(const byte_t target, gb_system_t *gb)
{
    byte_t result = target + 1;

    if (result == 0) reg_flag_set(FLAG_Z, gb); else reg_flag_clear(FLAG_Z, gb);
    reg_flag_clear(FLAG_N, gb);
    if (((target & 0xF) + 1) > 0xF) reg_flag_set(FLAG_H, gb); else reg_flag_clear(FLAG_H, gb);

    return result;
}

// INC n opcodes (registers only)
int opcode_inc_n_r(const opcode_t *opcode, gb_system_t *gb)
{
    byte_t reg;
    byte_t result;

    switch (opcode->opcode) {
        case 0x3C: reg = REG_A; break;
        case 0x04: reg = REG_B; break;
        case 0x0C: reg = REG_C; break;
        case 0x14: reg = REG_D; break;
        case 0x1C: reg = REG_E; break;
        case 0x24: reg = REG_H; break;
        case 0x2C: reg = REG_L; break;
        default: return OPCODE_ILLEGAL;
    }

    result = cpu_incb(reg_readb(reg, gb), gb);
    reg_writeb(reg, result, gb);

    return opcode->cycles_true;
}

// INC (HL)
int opcode_inc_n_hl(const opcode_t *opcode, gb_system_t *gb)
{
    uint16_t addr = reg_read_u16(REG_HL, gb);
    byte_t result = cpu_incb(mmu_readb(addr, gb), gb);

    mmu_writeb(addr, result, gb);
    return opcode->cycles_true;
}

// INC nn opcodes
int opcode_inc_nn(const opcode_t *opcode, gb_system_t *gb)
{
    switch (opcode->opcode) {
        case 0x03: reg_write_u16(REG_BC, reg_read_u16(REG_BC, gb) + 1, gb); break;
        case 0x13: reg_write_u16(REG_DE, reg_read_u16(REG_DE, gb) + 1, gb); break;
        case 0x23: reg_write_u16(REG_HL, reg_read_u16(REG_HL, gb) + 1, gb); break;
        case 0x33: gb->sp += 1; break;
        default: return OPCODE_ILLEGAL;
    }
    return opcode->cycles_true;
}