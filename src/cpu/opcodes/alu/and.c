/*
and.c
AND opcodes

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

// AND n opcodes
int opcode_and(const opcode_t *opcode, gb_system_t *gb)
{
    byte_t value;
    byte_t result;

    switch (opcode->opcode) {
        // AND r
        case 0xA7: value = reg_readb(REG_A, gb); break;
        case 0xA0: value = reg_readb(REG_B, gb); break;
        case 0xA1: value = reg_readb(REG_C, gb); break;
        case 0xA2: value = reg_readb(REG_D, gb); break;
        case 0xA3: value = reg_readb(REG_E, gb); break;
        case 0xA4: value = reg_readb(REG_H, gb); break;
        case 0xA5: value = reg_readb(REG_L, gb); break;

        // AND n
        case 0xE6: value = cpu_fetchb(gb); break;

        // AND (HL)
        case 0xA6: value = mmu_readb(reg_read_u16(REG_HL, gb), gb); break;

        default: return OPCODE_ILLEGAL;
    }

    result = reg_readb(REG_A, gb) & value;

    if (result == 0) reg_flag_set(FLAG_Z, gb); else reg_flag_clear(FLAG_Z, gb);
    reg_flag_clear(FLAG_N, gb);
    reg_flag_set(FLAG_H, gb);
    reg_flag_clear(FLAG_C, gb);

    reg_writeb(REG_A, result, gb);

    return opcode->cycles_true;
}