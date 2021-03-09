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
    const byte_t hresult = (target & 0xF) + (value & 0xF);
    const uint16_t result = target + value;

    gb->regs.f.flags.n = 0;
    gb->regs.f.flags.h = hresult > 0xF;
    gb->regs.f.flags.c = result > 0xFF;
    return (byte_t) (result & 0xFF);
}

// Add two uint16
// Affected flags
//     N reset
//     H carry from bit 11
//     C carry from bit 15
uint16_t cpu_add_u16(const uint16_t target, const uint16_t value, gb_system_t *gb)
{
    const uint16_t hresult = (target & 0xFFF) + (value & 0xFFF);
    const uint32_t result = target + value;

    gb->regs.f.flags.n = 0;
    gb->regs.f.flags.h = hresult > 0xFFF;
    gb->regs.f.flags.c = result > 0xFFFF;
    return (uint16_t) (result & 0xFFFF);
}

// Add signed byte e to SP
// Affected flags
//     Z reset
//     N reset
//     H carry/borrow from bit 3
//     C carry/borrow from bit 7
uint16_t cpu_add_sp_e(const sbyte_t e, gb_system_t *gb)
{
    const uint16_t result = gb->sp + e;

    gb->regs.f.flags.h = (result & 0xF) < (gb->sp & 0xF);
    gb->regs.f.flags.c = (result & 0xFF) < (gb->sp & 0xFF);
    gb->regs.f.flags.z = 0;
    gb->regs.f.flags.n = 0;
    return result;
}

// ADD A,n and ADD A,(HL) opcodes
// Zero Flag = (result == 0)
int opcode_add_a_n(const opcode_t *opcode, gb_system_t *gb)
{
    byte_t value;

    switch (opcode->opcode) {
        // ADD A,n
        case 0x87: value = gb->regs.a; break;
        case 0x80: value = gb->regs.b; break;
        case 0x81: value = gb->regs.c; break;
        case 0x82: value = gb->regs.d; break;
        case 0x83: value = gb->regs.e; break;
        case 0x84: value = gb->regs.h; break;
        case 0x85: value = gb->regs.l; break;
        case 0xC6: value = cpu_fetchb(gb); break;

        // ADD A,(HL)
        case 0x86: value = mmu_readb(reg_read_hl(gb), gb); break;
        default: return OPCODE_ILLEGAL;
    }
    gb->regs.a = cpu_addb(gb->regs.a, value, gb);
    gb->regs.f.flags.z = gb->regs.a == 0;
    return opcode->cycles_true;
}

// ADD HL,n opcodes
int opcode_add_hl_n(const opcode_t *opcode, gb_system_t *gb)
{
    uint16_t value;

    switch (opcode->opcode) {
        case 0x09: value = reg_read_bc(gb); break;
        case 0x19: value = reg_read_de(gb); break;
        case 0x29: value = reg_read_hl(gb); break;
        case 0x39: value = gb->sp; break;
        default: return OPCODE_ILLEGAL;
    }
    reg_write_hl(cpu_add_u16(reg_read_hl(gb), value, gb), gb);
    return opcode->cycles_true;
}

int opcode_add_sp_n(const opcode_t *opcode, gb_system_t *gb)
{
    gb->sp = cpu_add_sp_e((sbyte_t) cpu_fetchb(gb), gb);
    return opcode->cycles_true;
}