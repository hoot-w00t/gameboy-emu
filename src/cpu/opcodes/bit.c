/*
bit.c
BIT CB opcodes

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

static inline void cpu_test_bit(const byte_t target, const byte_t bit, gb_system_t *gb)
{
    gb->regs.f.flags.z = !(target & (1 << bit));
    gb->regs.f.flags.n = 0;
    gb->regs.f.flags.h = 1;
}

// BIT b,n opcodes
int opcode_cb_bit(const opcode_t *opcode, gb_system_t *gb)
{
    const byte_t bit = (opcode->opcode - 0x40) / 8;
    const byte_t op_lo = opcode->opcode % 8;

    switch (op_lo) {
        case 0: cpu_test_bit(gb->regs.b, bit, gb); return opcode->cycles_true;
        case 1: cpu_test_bit(gb->regs.c, bit, gb); return opcode->cycles_true;
        case 2: cpu_test_bit(gb->regs.d, bit, gb); return opcode->cycles_true;
        case 3: cpu_test_bit(gb->regs.e, bit, gb); return opcode->cycles_true;
        case 4: cpu_test_bit(gb->regs.h, bit, gb); return opcode->cycles_true;
        case 5: cpu_test_bit(gb->regs.l, bit, gb); return opcode->cycles_true;
        case 6:
            cpu_test_bit(mmu_read_u16(reg_read_hl(gb), gb), bit, gb);
            return opcode->cycles_true;
        case 7: cpu_test_bit(gb->regs.a, bit, gb); return opcode->cycles_true;
        default: return OPCODE_ILLEGAL;
    }
}