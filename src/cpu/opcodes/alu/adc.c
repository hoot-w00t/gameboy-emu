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

// Add two bytes + Carry
// Affected flags
//     N reset
//     H carry from bit 3
//     C carry from bit 7
byte_t cpu_adc(const byte_t target, const byte_t value, gb_system_t *gb)
{
    const byte_t hresult = (target & 0xF) + (value & 0xF) + gb->regs.f.flags.c;
    const uint16_t result = target + value + gb->regs.f.flags.c;

    gb->regs.f.flags.n = 0;
    gb->regs.f.flags.h = hresult > 0xF;
    gb->regs.f.flags.c = result > 0xFF;
    return (byte_t) result;
}

// ADC A,n and ADC A,(HL) opcodes
// Zero Flag = (result == 0)
int opcode_adc_a_n(const opcode_t *opcode, gb_system_t *gb)
{
    byte_t value;

    switch (opcode->opcode) {
        // ADC A,n
        case 0x8F: value = gb->regs.a; break;
        case 0x88: value = gb->regs.b; break;
        case 0x89: value = gb->regs.c; break;
        case 0x8A: value = gb->regs.d; break;
        case 0x8B: value = gb->regs.e; break;
        case 0x8C: value = gb->regs.h; break;
        case 0x8D: value = gb->regs.l; break;
        case 0xCE: value = cpu_fetchb(gb); break;

        // ADC A,(HL)
        case 0x8E: value = mmu_readb(reg_read_hl(gb), gb); break;
        default: return OPCODE_ILLEGAL;
    }
    gb->regs.a = cpu_adc(gb->regs.a, value, gb);
    gb->regs.f.flags.z = gb->regs.a == 0;
    return opcode->cycles_true;
}