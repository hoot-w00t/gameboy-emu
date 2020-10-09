/*
or.c
OR opcodes

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

// OR n opcodes
int opcode_or(const opcode_t *opcode, gb_system_t *gb)
{
    byte_t value;
    byte_t result;

    switch (opcode->opcode) {
        // OR r
        case 0xB7: value = reg_readb(REG_A, gb); break;
        case 0xB0: value = reg_readb(REG_B, gb); break;
        case 0xB1: value = reg_readb(REG_C, gb); break;
        case 0xB2: value = reg_readb(REG_D, gb); break;
        case 0xB3: value = reg_readb(REG_E, gb); break;
        case 0xB4: value = reg_readb(REG_H, gb); break;
        case 0xB5: value = reg_readb(REG_L, gb); break;

        // OR n
        case 0xF6: value = cpu_fetchb(gb); break;

        // OR (HL)
        case 0xB6: value = mmu_readb(reg_read_u16(REG_HL, gb), gb); break;

        default: return OPCODE_ILLEGAL;
    }

    result = reg_readb(REG_A, gb) | value;

    if (result == 0) reg_flag_set(FLAG_Z, gb); else reg_flag_clear(FLAG_Z, gb);
    reg_flag_clear(FLAG_N, gb);
    reg_flag_clear(FLAG_H, gb);
    reg_flag_clear(FLAG_C, gb);

    reg_writeb(REG_A, result, gb);

    return opcode->cycles_true;
}