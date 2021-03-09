/*
dec.c
DEC opcodes

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

// Decrement byte
// Affected flags
//     Z if result == 0
//     N set
//     H if no borrow from bit 4
byte_t cpu_decb(const byte_t target, gb_system_t *gb)
{
    gb->regs.f.flags.z = target == 1;
    gb->regs.f.flags.n = 1;
    gb->regs.f.flags.h = (target & 0xF) == 0;
    return (byte_t) (target - 1);
}

// DEC n opcodes (registers only)
int opcode_dec_n_r(const opcode_t *opcode, gb_system_t *gb)
{
    switch (opcode->opcode) {
        case 0x3D: gb->regs.a = cpu_decb(gb->regs.a, gb); return opcode->cycles_true;
        case 0x05: gb->regs.b = cpu_decb(gb->regs.b, gb); return opcode->cycles_true;
        case 0x0D: gb->regs.c = cpu_decb(gb->regs.c, gb); return opcode->cycles_true;
        case 0x15: gb->regs.d = cpu_decb(gb->regs.d, gb); return opcode->cycles_true;
        case 0x1D: gb->regs.e = cpu_decb(gb->regs.e, gb); return opcode->cycles_true;
        case 0x25: gb->regs.h = cpu_decb(gb->regs.h, gb); return opcode->cycles_true;
        case 0x2D: gb->regs.l = cpu_decb(gb->regs.l, gb); return opcode->cycles_true;
        default: return OPCODE_ILLEGAL;
    }
}

// DEC (HL)
int opcode_dec_n_hl(const opcode_t *opcode, gb_system_t *gb)
{
    const uint16_t addr = reg_read_hl(gb);

    mmu_writeb(addr, cpu_decb(mmu_readb(addr, gb), gb), gb);
    return opcode->cycles_true;
}

// DEC nn opcodes
int opcode_dec_nn(const opcode_t *opcode, gb_system_t *gb)
{
    switch (opcode->opcode) {
        case 0x0B: reg_write_bc(reg_read_bc(gb) - 1, gb); return opcode->cycles_true;
        case 0x1B: reg_write_de(reg_read_de(gb) - 1, gb); return opcode->cycles_true;
        case 0x2B: reg_write_hl(reg_read_hl(gb) - 1, gb); return opcode->cycles_true;
        case 0x3B: gb->sp -= 1; return opcode->cycles_true;
        default: return OPCODE_ILLEGAL;
    }
}