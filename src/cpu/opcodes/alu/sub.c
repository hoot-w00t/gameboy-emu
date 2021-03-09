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
    gb->regs.f.flags.n = 1;
    gb->regs.f.flags.h = (target & 0xF) < (value & 0xF);
    gb->regs.f.flags.c = target < value;
    return (byte_t) (target - value);
}

// Subtract two uint16
// Affected flags
//     N set
//     H if no borrow from bit 11
//     C if no borrow
uint16_t cpu_sub_u16(const uint16_t target, const uint16_t value, gb_system_t *gb)
{
    gb->regs.f.flags.n = 1;
    gb->regs.f.flags.h = (target & 0xFFF) < (value & 0xFFF);
    gb->regs.f.flags.c = target < value;
    return (byte_t) (target - value);
}

// SUB A,n opcodes
// Zero Flag = (result == 0)
int opcode_sub(const opcode_t *opcode, gb_system_t *gb)
{
    byte_t value;

    switch (opcode->opcode) {
        // SUB A,r
        case 0x97: value = gb->regs.a; break;
        case 0x90: value = gb->regs.b; break;
        case 0x91: value = gb->regs.c; break;
        case 0x92: value = gb->regs.d; break;
        case 0x93: value = gb->regs.e; break;
        case 0x94: value = gb->regs.h; break;
        case 0x95: value = gb->regs.l; break;

        // SUB A,n
        case 0xD6: value = cpu_fetchb(gb); break;

        // SUB A,(HL)
        case 0x96: value = mmu_readb(reg_read_hl(gb), gb); break;
        default: return OPCODE_ILLEGAL;
    }

    gb->regs.a = cpu_subb(gb->regs.a, value, gb);
    gb->regs.f.flags.z = gb->regs.a == 0;
    return opcode->cycles_true;
}