/*
rotate.c
RR and RL opcodes

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

// Rotate Left
static inline byte_t cpu_rlc(const byte_t value, gb_system_t *gb)
{
    const byte_t result = (value << 1) | (value >> 7);

    gb->regs.f.flags.z = result == 0;
    gb->regs.f.flags.n = 0;
    gb->regs.f.flags.h = 0;
    gb->regs.f.flags.c = (value >> 7);
    return result;
}

// Rotate Left through carry
static inline byte_t cpu_rl(const byte_t value, gb_system_t *gb)
{
    const byte_t result = (value << 1) | gb->regs.f.flags.c;

    gb->regs.f.flags.z = result == 0;
    gb->regs.f.flags.n = 0;
    gb->regs.f.flags.h = 0;
    gb->regs.f.flags.c = (value >> 7);
    return result;
}

// Rotate Right
static inline byte_t cpu_rrc(const byte_t value, gb_system_t *gb)
{
    const byte_t result = (value >> 1) | (value << 7);

    gb->regs.f.flags.z = result == 0;
    gb->regs.f.flags.n = 0;
    gb->regs.f.flags.h = 0;
    gb->regs.f.flags.c = (value & 0x1);
    return result;
}

// Rotate Right through carry
static inline byte_t cpu_rr(const byte_t value, gb_system_t *gb)
{
    const byte_t result = (value >> 1) | (gb->regs.f.flags.c << 7);

    gb->regs.f.flags.z = result == 0;
    gb->regs.f.flags.n = 0;
    gb->regs.f.flags.h = 0;
    gb->regs.f.flags.c = (value & 0x1);
    return result;
}

// RLA, RLCA, RRA, RRCA opcodes
int opcode_rotate_a(const opcode_t *opcode, gb_system_t *gb)
{
    switch (opcode->opcode) {
        // RLCA
        case 0x07: gb->regs.a = cpu_rlc(gb->regs.a, gb); break;
        // RLA
        case 0x17: gb->regs.a = cpu_rl(gb->regs.a, gb); break;
        // RRCA
        case 0x0F: gb->regs.a = cpu_rrc(gb->regs.a, gb); break;
        // RRA
        case 0x1F: gb->regs.a = cpu_rr(gb->regs.a, gb); break;
        default: return OPCODE_ILLEGAL;
    }
    gb->regs.f.flags.z = 0;
    return opcode->cycles_true;
}

// (CB) RL/RLC/RR/RRC (HL)
int opcode_cb_rotate_n(const opcode_t *opcode, gb_system_t *gb)
{
    const uint16_t addr = reg_read_hl(gb);
    byte_t result;

    switch (opcode->opcode) {
        // RLC (HL)
        case 0x06: result = cpu_rlc(mmu_readb(addr, gb), gb); break;
        // RL (HL)
        case 0x16: result = cpu_rl(mmu_readb(addr, gb), gb); break;
        // RRC (HL)
        case 0x0E: result = cpu_rrc(mmu_readb(addr, gb), gb); break;
        // RR (HL)
        case 0x1E: result = cpu_rr(mmu_readb(addr, gb), gb); break;
        default: return OPCODE_ILLEGAL;
    }
    mmu_writeb(addr, result, gb);
    return opcode->cycles_true;
}

// (CB) RLC r
int opcode_cb_rlc_r(const opcode_t *opcode, gb_system_t *gb)
{
    switch (opcode->opcode) {
        case 0x07: gb->regs.a = cpu_rlc(gb->regs.a, gb); return opcode->cycles_true;
        case 0x00: gb->regs.b = cpu_rlc(gb->regs.b, gb); return opcode->cycles_true;
        case 0x01: gb->regs.c = cpu_rlc(gb->regs.c, gb); return opcode->cycles_true;
        case 0x02: gb->regs.d = cpu_rlc(gb->regs.d, gb); return opcode->cycles_true;
        case 0x03: gb->regs.e = cpu_rlc(gb->regs.e, gb); return opcode->cycles_true;
        case 0x04: gb->regs.h = cpu_rlc(gb->regs.h, gb); return opcode->cycles_true;
        case 0x05: gb->regs.l = cpu_rlc(gb->regs.l, gb); return opcode->cycles_true;
        default: return OPCODE_ILLEGAL;
    }
}

// (CB) RL r
int opcode_cb_rl_r(const opcode_t *opcode, gb_system_t *gb)
{
    switch (opcode->opcode) {
        case 0x17: gb->regs.a = cpu_rl(gb->regs.a, gb); return opcode->cycles_true;
        case 0x10: gb->regs.b = cpu_rl(gb->regs.b, gb); return opcode->cycles_true;
        case 0x11: gb->regs.c = cpu_rl(gb->regs.c, gb); return opcode->cycles_true;
        case 0x12: gb->regs.d = cpu_rl(gb->regs.d, gb); return opcode->cycles_true;
        case 0x13: gb->regs.e = cpu_rl(gb->regs.e, gb); return opcode->cycles_true;
        case 0x14: gb->regs.h = cpu_rl(gb->regs.h, gb); return opcode->cycles_true;
        case 0x15: gb->regs.l = cpu_rl(gb->regs.l, gb); return opcode->cycles_true;
        default: return OPCODE_ILLEGAL;
    }
}

// (CB) RRC r
int opcode_cb_rrc_r(const opcode_t *opcode, gb_system_t *gb)
{
    switch (opcode->opcode) {
        case 0x0F: gb->regs.a = cpu_rrc(gb->regs.a, gb); return opcode->cycles_true;
        case 0x08: gb->regs.b = cpu_rrc(gb->regs.b, gb); return opcode->cycles_true;
        case 0x09: gb->regs.c = cpu_rrc(gb->regs.c, gb); return opcode->cycles_true;
        case 0x0A: gb->regs.d = cpu_rrc(gb->regs.d, gb); return opcode->cycles_true;
        case 0x0B: gb->regs.e = cpu_rrc(gb->regs.e, gb); return opcode->cycles_true;
        case 0x0C: gb->regs.h = cpu_rrc(gb->regs.h, gb); return opcode->cycles_true;
        case 0x0D: gb->regs.l = cpu_rrc(gb->regs.l, gb); return opcode->cycles_true;
        default: return OPCODE_ILLEGAL;
    }
}

// (CB) RR r
int opcode_cb_rr_r(const opcode_t *opcode, gb_system_t *gb)
{
    switch (opcode->opcode) {
        case 0x1F: gb->regs.a = cpu_rr(gb->regs.a, gb); return opcode->cycles_true;
        case 0x18: gb->regs.b = cpu_rr(gb->regs.b, gb); return opcode->cycles_true;
        case 0x19: gb->regs.c = cpu_rr(gb->regs.c, gb); return opcode->cycles_true;
        case 0x1A: gb->regs.d = cpu_rr(gb->regs.d, gb); return opcode->cycles_true;
        case 0x1B: gb->regs.e = cpu_rr(gb->regs.e, gb); return opcode->cycles_true;
        case 0x1C: gb->regs.h = cpu_rr(gb->regs.h, gb); return opcode->cycles_true;
        case 0x1D: gb->regs.l = cpu_rr(gb->regs.l, gb); return opcode->cycles_true;
        default: return OPCODE_ILLEGAL;
    }
}