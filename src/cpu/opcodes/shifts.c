/*
shifts.c
SLA, SRA and SRL opcodes

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

static inline byte_t cpu_shift_left(const byte_t target, gb_system_t *gb)
{
    const byte_t result = target << 1;

    gb->regs.f.flags.z = result == 0;
    gb->regs.f.flags.n = 0;
    gb->regs.f.flags.h = 0;
    gb->regs.f.flags.c = target >> 7;
    return result;
}

// If keep_msb is true, bit 7 doesn't change
static inline byte_t cpu_shift_right(const byte_t target, const bool keep_msb, gb_system_t *gb)
{
    byte_t result = target >> 1;

    if (keep_msb && (target & 0x80))
        result |= 0x80;
    gb->regs.f.flags.z = result == 0;
    gb->regs.f.flags.n = 0;
    gb->regs.f.flags.h = 0;
    gb->regs.f.flags.c = (target & 1);
    return result;
}

// SLA/SRA/SRL (HL)
int opcode_cb_shift_hl(const opcode_t *opcode, gb_system_t *gb)
{
    uint16_t addr = reg_read_hl(gb);

    switch (opcode->opcode) {
        // SLA (HL)
        case 0x26:
            mmu_writeb(addr, cpu_shift_left(mmu_readb(addr, gb), gb), gb);
            return opcode->cycles_true;

        // SRA (HL)
        case 0x2E:
            mmu_writeb(addr, cpu_shift_right(mmu_readb(addr, gb), true, gb), gb);
            return opcode->cycles_true;

        // SRL (HL)
        case 0x3E:
            mmu_writeb(addr, cpu_shift_right(mmu_readb(addr, gb), false, gb), gb);
            return opcode->cycles_true;;

        default: return OPCODE_ILLEGAL;
    }
}

// SLA r
int opcode_cb_sla_r(const opcode_t *opcode, gb_system_t *gb)
{
    byte_t *reg;

    switch (opcode->opcode) {
        case 0x27: reg = &gb->regs.a; break;
        case 0x20: reg = &gb->regs.b; break;
        case 0x21: reg = &gb->regs.c; break;
        case 0x22: reg = &gb->regs.d; break;
        case 0x23: reg = &gb->regs.e; break;
        case 0x24: reg = &gb->regs.h; break;
        case 0x25: reg = &gb->regs.l; break;
        default: return OPCODE_ILLEGAL;
    }
    *reg = cpu_shift_left(*reg, gb);
    return opcode->cycles_true;
}

// SRA r
int opcode_cb_sra_r(const opcode_t *opcode, gb_system_t *gb)
{
    byte_t *reg;

    switch (opcode->opcode) {
        case 0x2F: reg = &gb->regs.a; break;
        case 0x28: reg = &gb->regs.b; break;
        case 0x29: reg = &gb->regs.c; break;
        case 0x2A: reg = &gb->regs.d; break;
        case 0x2B: reg = &gb->regs.e; break;
        case 0x2C: reg = &gb->regs.h; break;
        case 0x2D: reg = &gb->regs.l; break;
        default: return OPCODE_ILLEGAL;
    }
    *reg = cpu_shift_right(*reg, true, gb);
    return opcode->cycles_true;
}

// SRL r
int opcode_cb_srl_r(const opcode_t *opcode, gb_system_t *gb)
{
    byte_t *reg;

    switch (opcode->opcode) {
        case 0x3F: reg = &gb->regs.a; break;
        case 0x38: reg = &gb->regs.b; break;
        case 0x39: reg = &gb->regs.c; break;
        case 0x3A: reg = &gb->regs.d; break;
        case 0x3B: reg = &gb->regs.e; break;
        case 0x3C: reg = &gb->regs.h; break;
        case 0x3D: reg = &gb->regs.l; break;
        default: return OPCODE_ILLEGAL;
    }
    *reg = cpu_shift_right(*reg, false, gb);
    return opcode->cycles_true;
}