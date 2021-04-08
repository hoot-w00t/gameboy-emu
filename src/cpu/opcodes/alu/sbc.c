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
    const int16_t result = target - value - gb->regs.f.flags.c;
    const int16_t hresult = (target & 0xF) - (value & 0xF) - gb->regs.f.flags.c;

    gb->regs.f.flags.n = 1;
    gb->regs.f.flags.h = hresult < 0;
    gb->regs.f.flags.c = result < 0;
    return result & 0xFF;
}

// SBC A,n opcodes
// Zero Flag = (result == 0)
int opcode_sbc(const opcode_t *opcode, gb_system_t *gb)
{
    byte_t value;

    switch (opcode->opcode) {
        // SBC A,r
        case 0x9F: value = gb->regs.a; break;
        case 0x98: value = gb->regs.b; break;
        case 0x99: value = gb->regs.c; break;
        case 0x9A: value = gb->regs.d; break;
        case 0x9B: value = gb->regs.e; break;
        case 0x9C: value = gb->regs.h; break;
        case 0x9D: value = gb->regs.l; break;

        // SBC A,n
        case 0xDE: value = cpu_fetchb(gb); break;

        // SBC A,(HL)
        case 0x9E: value = mmu_readb(reg_read_hl(gb), gb); break;

        default: return OPCODE_ILLEGAL;
    }

    gb->regs.a = cpu_sbc(gb->regs.a, value, gb);
    gb->regs.f.flags.z = gb->regs.a == 0;
    return opcode->cycles_true;
}