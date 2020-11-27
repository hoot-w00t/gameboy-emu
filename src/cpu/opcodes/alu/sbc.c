/*
sbc.c
SBC opcodes

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

// Subtract two bytes + Carry
// Affected flags
//     N set
//     H if no borrow from bit 4
//     C if no borrow
byte_t cpu_sbc(const byte_t target, const byte_t value, gb_system_t *gb)
{
    byte_t cvalue = reg_flag(FLAG_C, gb) ? 1 : 0;
    int16_t result = target - value - cvalue;
    int16_t hresult = (target & 0xF) - (value & 0xF) - cvalue;

    reg_flag_set(FLAG_N, gb);
    if (hresult < 0) reg_flag_set(FLAG_H, gb); else reg_flag_clear(FLAG_H, gb);
    if (result < 0) reg_flag_set(FLAG_C, gb); else reg_flag_clear(FLAG_C, gb);

    return result & 0xFF;
}

// SBC A,n opcodes
// Zero Flag = (result == 0)
int opcode_sbc(const opcode_t *opcode, gb_system_t *gb)
{
    byte_t value;
    byte_t result;

    switch (opcode->opcode) {
        // SBC A,r
        case 0x9F: value = reg_readb(REG_A, gb); break;
        case 0x98: value = reg_readb(REG_B, gb); break;
        case 0x99: value = reg_readb(REG_C, gb); break;
        case 0x9A: value = reg_readb(REG_D, gb); break;
        case 0x9B: value = reg_readb(REG_E, gb); break;
        case 0x9C: value = reg_readb(REG_H, gb); break;
        case 0x9D: value = reg_readb(REG_L, gb); break;

        // SBC A,n
        case 0xDE: value = cpu_fetchb(gb); break;

        // SBC A,(HL)
        case 0x9E: value = mmu_readb(reg_read_u16(REG_HL, gb), gb); break;

        default: return OPCODE_ILLEGAL;
    }

    result = cpu_sbc(reg_readb(REG_A, gb), value, gb);
    if (result == 0) reg_flag_set(FLAG_Z, gb); else reg_flag_clear(FLAG_Z, gb);
    reg_writeb(REG_A, result, gb);

    return opcode->cycles_true;
}