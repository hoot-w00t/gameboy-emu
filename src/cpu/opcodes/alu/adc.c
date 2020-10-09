/*
adc.c
ADC opcodes

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
#include "cpu/opcodes/alu/add.h"
#include "mmu/mmu.h"

// ADC A,n and ADC A,(HL) opcodes
// Zero Flag = (result == 0)
int opcode_adc_a_n(const opcode_t *opcode, gb_system_t *gb)
{
    byte_t value;
    byte_t result;

    if (reg_flag(FLAG_C, gb)) value = 1; else value = 0;

    switch (opcode->opcode) {
        // ADC A,n
        case 0x8F: value = reg_readb(REG_A, gb); break;
        case 0x88: value = reg_readb(REG_B, gb); break;
        case 0x89: value = reg_readb(REG_C, gb); break;
        case 0x8A: value = reg_readb(REG_D, gb); break;
        case 0x8B: value = reg_readb(REG_E, gb); break;
        case 0x8C: value = reg_readb(REG_H, gb); break;
        case 0x8D: value = reg_readb(REG_L, gb); break;
        case 0xCE: value = cpu_fetchb(gb); break;

        // ADC A,(HL)
        case 0x8E: value = mmu_readb(reg_read_u16(REG_HL, gb), gb); break;

        default: return OPCODE_ILLEGAL;
    }

    result = cpu_addb(reg_readb(REG_A, gb), value, gb);
    if (result == 0) reg_flag_set(FLAG_Z, gb); else reg_flag_clear(FLAG_Z, gb);
    reg_writeb(REG_A, result, gb);

    return opcode->cycles_true;
}