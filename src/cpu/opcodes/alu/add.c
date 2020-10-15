/*
add.c
ADD opcodes

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
#include "cpu/opcodes/alu/sub.h"
#include "mmu/mmu.h"

// Add two bytes
// Affected flags
//     N reset
//     H carry from bit 3
//     C carry from bit 7
byte_t cpu_addb(const byte_t target, const byte_t value, gb_system_t *gb)
{
    byte_t hresult = (target & 0xF) + (value & 0xF);
    uint16_t result = target + value;

    reg_flag_clear(FLAG_N, gb);
    if (hresult > 0xF) reg_flag_set(FLAG_H, gb); else reg_flag_clear(FLAG_H, gb);
    if (result > 0xFF) reg_flag_set(FLAG_C, gb); else reg_flag_clear(FLAG_C, gb);

    return (byte_t) (result & 0xFF);
}

// Add two uint16
// Affected flags
//     N reset
//     H carry from bit 11
//     C carry from bit 15
uint16_t cpu_add_u16(const uint16_t target, const uint16_t value, gb_system_t *gb)
{
    uint16_t hresult = (target & 0xFFF) + (value & 0xFFF);
    uint32_t result = target + value;

    reg_flag_clear(FLAG_N, gb);
    if (hresult > 0xFFF) reg_flag_set(FLAG_H, gb); else reg_flag_clear(FLAG_H, gb);
    if (result > 0xFFFF) reg_flag_set(FLAG_C, gb); else reg_flag_clear(FLAG_C, gb);

    return (uint16_t) (result & 0xFFFF);
}

// Add signed byte to uint16
// Affected flags
//     Z reset
//     N reset
//     H carry/borrow from bit 11
//     C carry/borrow from bit 15
uint16_t cpu_add_sb_u16(const uint16_t target, const sbyte_t value, gb_system_t *gb)
{
    uint16_t result;

    if (value >= 0) {
        result = cpu_add_u16(target, value, gb);
    } else {
        result = cpu_sub_u16(target, -value, gb);
    }
    reg_flag_clear(FLAG_Z, gb);
    reg_flag_clear(FLAG_N, gb);

    return result;
}

// ADD A,n and ADD A,(HL) opcodes
// Zero Flag = (result == 0)
int opcode_add_a_n(const opcode_t *opcode, gb_system_t *gb)
{
    byte_t value;
    byte_t result;

    switch (opcode->opcode) {
        // ADD A,n
        case 0x87: value = reg_readb(REG_A, gb); break;
        case 0x80: value = reg_readb(REG_B, gb); break;
        case 0x81: value = reg_readb(REG_C, gb); break;
        case 0x82: value = reg_readb(REG_D, gb); break;
        case 0x83: value = reg_readb(REG_E, gb); break;
        case 0x84: value = reg_readb(REG_H, gb); break;
        case 0x85: value = reg_readb(REG_L, gb); break;
        case 0xC6: value = cpu_fetchb(gb); break;

        // ADD A,(HL)
        case 0x86: value = mmu_readb(reg_read_u16(REG_HL, gb), gb); break;

        default: return OPCODE_ILLEGAL;
    }

    result = cpu_addb(reg_readb(REG_A, gb), value, gb);
    if (result == 0) reg_flag_set(FLAG_Z, gb); else reg_flag_clear(FLAG_Z, gb);
    reg_writeb(REG_A, result, gb);

    return opcode->cycles_true;
}

// ADD HL,n opcodes
int opcode_add_hl_n(const opcode_t *opcode, gb_system_t *gb)
{
    uint16_t value;
    uint16_t result;

    switch (opcode->opcode) {
        case 0x09: value = reg_read_u16(REG_BC, gb); break;
        case 0x19: value = reg_read_u16(REG_DE, gb); break;
        case 0x29: value = reg_read_u16(REG_HL, gb); break;
        case 0x39: value = gb->sp; break;
        default: return OPCODE_ILLEGAL;
    }

    result = cpu_add_u16(reg_read_u16(REG_HL, gb), value, gb);
    reg_write_u16(REG_HL, result, gb);

    return opcode->cycles_true;
}

// TODO: Implement ADD SP,n (0xE8) when cpu_sub() is implemented