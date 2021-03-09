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

    switch (opcode->opcode) {
        // AND r
        case 0xA7: value = gb->regs.a; break;
        case 0xA0: value = gb->regs.b; break;
        case 0xA1: value = gb->regs.c; break;
        case 0xA2: value = gb->regs.d; break;
        case 0xA3: value = gb->regs.e; break;
        case 0xA4: value = gb->regs.h; break;
        case 0xA5: value = gb->regs.l; break;

        // AND n
        case 0xE6: value = cpu_fetchb(gb); break;

        // AND (HL)
        case 0xA6: value = mmu_readb(reg_read_hl(gb), gb); break;
        default: return OPCODE_ILLEGAL;
    }

    gb->regs.a &= value;
    gb->regs.f.flags.z = gb->regs.a == 0;
    gb->regs.f.flags.n = 0;
    gb->regs.f.flags.h = 1;
    gb->regs.f.flags.c = 0;
    return opcode->cycles_true;
}