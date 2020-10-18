/*
sub.c
SUB opcodes

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
#include "cpu/cpu.h"
#include "cpu/registers.h"
#include "mmu/mmu.h"

// Subtract two bytes
// Affected flags
//     N set
//     H if no borrow from bit 4
//     C if no borrow
byte_t cpu_subb(const byte_t target, const byte_t value, gb_system_t *gb)
{
    byte_t result = target - value;

    reg_flag_set(FLAG_N, gb);
    if ((target & 0xF) < (value & 0xF)) reg_flag_clear(FLAG_H, gb); else reg_flag_set(FLAG_H, gb);
    if (target < value) reg_flag_clear(FLAG_C, gb); else reg_flag_set(FLAG_C, gb);

    return result;
}

// Subtract two uint16
// Affected flags
//     N set
//     H if no borrow from bit 11
//     C if no borrow
uint16_t cpu_sub_u16(const uint16_t target, const uint16_t value, gb_system_t *gb)
{
    uint16_t result = target - value;

    reg_flag_set(FLAG_N, gb);
    if ((target & 0xFFF) < (value & 0xFFF)) reg_flag_clear(FLAG_H, gb); else reg_flag_set(FLAG_H, gb);
    if (target < value) reg_flag_clear(FLAG_C, gb); else reg_flag_set(FLAG_C, gb);

    return result;
}

// SUB A,n opcodes
// Zero Flag = (result == 0)
int opcode_sub(const opcode_t *opcode, gb_system_t *gb)
{
    byte_t value;
    byte_t result;

    switch (opcode->opcode) {
        // SUB A,r
        case 0x97: value = reg_readb(REG_A, gb); break;
        case 0x90: value = reg_readb(REG_B, gb); break;
        case 0x91: value = reg_readb(REG_C, gb); break;
        case 0x92: value = reg_readb(REG_D, gb); break;
        case 0x93: value = reg_readb(REG_E, gb); break;
        case 0x94: value = reg_readb(REG_H, gb); break;
        case 0x95: value = reg_readb(REG_L, gb); break;

        // SUB A,n
        case 0xD6: value = cpu_fetchb(gb); break;

        // SUB A,(HL)
        case 0x96: value = mmu_readb(reg_read_u16(REG_HL, gb), gb); break;

        default: return OPCODE_ILLEGAL;
    }

    result = cpu_subb(reg_readb(REG_A, gb), value, gb);
    if (result == 0) reg_flag_set(FLAG_Z, gb); else reg_flag_clear(FLAG_Z, gb);
    reg_writeb(REG_A, result, gb);

    return opcode->cycles_true;
}