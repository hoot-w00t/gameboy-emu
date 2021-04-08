/*
inc.c
INC opcodes

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
#include "cpu/registers.h"
#include "mmu/mmu.h"

// Increment byte
// Affected flags
//     Z set if result == 0
//     N reset
//     H carry from bit 3
byte_t cpu_incb(const byte_t target, gb_system_t *gb)
{
    gb->regs.f.flags.z = target == 0xFF;
    gb->regs.f.flags.n = 0;
    gb->regs.f.flags.h = (target & 0xF) == 0xF;
    return (byte_t) (target + 1);
}

// INC n opcodes (registers only)
int opcode_inc_n_r(const opcode_t *opcode, gb_system_t *gb)
{
    switch (opcode->opcode) {
        case 0x3C: gb->regs.a = cpu_incb(gb->regs.a, gb); return opcode->cycles_true;
        case 0x04: gb->regs.b = cpu_incb(gb->regs.b, gb); return opcode->cycles_true;
        case 0x0C: gb->regs.c = cpu_incb(gb->regs.c, gb); return opcode->cycles_true;
        case 0x14: gb->regs.d = cpu_incb(gb->regs.d, gb); return opcode->cycles_true;
        case 0x1C: gb->regs.e = cpu_incb(gb->regs.e, gb); return opcode->cycles_true;
        case 0x24: gb->regs.h = cpu_incb(gb->regs.h, gb); return opcode->cycles_true;
        case 0x2C: gb->regs.l = cpu_incb(gb->regs.l, gb); return opcode->cycles_true;
        default: return OPCODE_ILLEGAL;
    }
}

// INC (HL)
int opcode_inc_n_hl(const opcode_t *opcode, gb_system_t *gb)
{
    const uint16_t addr = reg_read_hl(gb);

    mmu_writeb(addr, cpu_incb(mmu_readb(addr, gb), gb), gb);
    return opcode->cycles_true;
}

// INC nn opcodes
int opcode_inc_nn(const opcode_t *opcode, gb_system_t *gb)
{
    switch (opcode->opcode) {
        case 0x03: reg_write_bc(reg_read_bc(gb) + 1, gb); return opcode->cycles_true;
        case 0x13: reg_write_de(reg_read_de(gb) + 1, gb); return opcode->cycles_true;
        case 0x23: reg_write_hl(reg_read_hl(gb) + 1, gb); return opcode->cycles_true;
        case 0x33: gb->sp += 1; return opcode->cycles_true;
        default: return OPCODE_ILLEGAL;
    }
}