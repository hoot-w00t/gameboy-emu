/*
cp.c
CP opcodes

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

// CP n opcodes
int opcode_cp(const opcode_t *opcode, gb_system_t *gb)
{
    byte_t value;

    switch (opcode->opcode) {
        // CP A,r
        case 0xBF: value = reg_readb(REG_A, gb); break;
        case 0xB8: value = reg_readb(REG_B, gb); break;
        case 0xB9: value = reg_readb(REG_C, gb); break;
        case 0xBA: value = reg_readb(REG_D, gb); break;
        case 0xBB: value = reg_readb(REG_E, gb); break;
        case 0xBC: value = reg_readb(REG_H, gb); break;
        case 0xBD: value = reg_readb(REG_L, gb); break;

        // CP A,n
        case 0xFE: value = cpu_fetchb(gb); break;

        // CP A,(HL)
        case 0xBE: value = mmu_readb(reg_read_u16(REG_HL, gb), gb); break;

        default: return OPCODE_ILLEGAL;
    }

    if (cpu_subb(reg_readb(REG_A, gb), value, gb) == 0) {
        reg_flag_set(FLAG_Z, gb);
    } else {
        reg_flag_clear(FLAG_Z, gb);
    }

    return opcode->cycles_true;
}