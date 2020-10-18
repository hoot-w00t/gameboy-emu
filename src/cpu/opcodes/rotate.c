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
    byte_t out = value >> 7;
    byte_t result = (value << 1) | out;

    if (result == 0) reg_flag_set(FLAG_Z, gb); else reg_flag_clear(FLAG_Z, gb);
    reg_flag_clear(FLAG_N, gb);
    reg_flag_clear(FLAG_H, gb);
    if (out) reg_flag_set(FLAG_C, gb); else reg_flag_clear(FLAG_C, gb);

    return result;
}

// Rotate Left through carry
static inline byte_t cpu_rl(const byte_t value, gb_system_t *gb)
{
    byte_t out = value >> 7;
    byte_t result = value << 1;

    if (reg_flag(FLAG_C, gb)) result |= 1;

    if (result == 0) reg_flag_set(FLAG_Z, gb); else reg_flag_clear(FLAG_Z, gb);
    reg_flag_clear(FLAG_N, gb);
    reg_flag_clear(FLAG_H, gb);
    if (out) reg_flag_set(FLAG_C, gb); else reg_flag_clear(FLAG_C, gb);

    return result;
}

// Rotate Right
static inline byte_t cpu_rrc(const byte_t value, gb_system_t *gb)
{
    byte_t out = value << 7;
    byte_t result = (value >> 1) | out;

    if (result == 0) reg_flag_set(FLAG_Z, gb); else reg_flag_clear(FLAG_Z, gb);
    reg_flag_clear(FLAG_N, gb);
    reg_flag_clear(FLAG_H, gb);
    if (out) reg_flag_set(FLAG_C, gb); else reg_flag_clear(FLAG_C, gb);

    return result;
}

// Rotate Right through carry
static inline byte_t cpu_rr(const byte_t value, gb_system_t *gb)
{
    byte_t out = value << 7;
    byte_t result = value >> 1;

    if (reg_flag(FLAG_C, gb)) result |= 0x80;

    if (result == 0) reg_flag_set(FLAG_Z, gb); else reg_flag_clear(FLAG_Z, gb);
    reg_flag_clear(FLAG_N, gb);
    reg_flag_clear(FLAG_H, gb);
    if (out) reg_flag_set(FLAG_C, gb); else reg_flag_clear(FLAG_C, gb);

    return result;
}

// RLA, RLCA, RRA, RRCA opcodes
int opcode_rotate_a(const opcode_t *opcode, gb_system_t *gb)
{
    byte_t result;

    switch (opcode->opcode) {
        // RLCA
        case 0x07: result = cpu_rlc(reg_readb(REG_A, gb), gb); break;

        // RLA
        case 0x17: result = cpu_rl(reg_readb(REG_A, gb), gb); break;

        // RRCA
        case 0x0F: result = cpu_rrc(reg_readb(REG_A, gb), gb); break;

        // RRA
        case 0x1F: result = cpu_rr(reg_readb(REG_A, gb), gb); break;

        default: return OPCODE_ILLEGAL;
    }

    reg_writeb(REG_A, result, gb);
    reg_flag_clear(FLAG_Z, gb);

    return opcode->cycles_true;
}

// (CB) RL/RLC/RR/RRC (HL)
int opcode_cb_rotate_n(const opcode_t *opcode, gb_system_t *gb)
{
    uint16_t addr = reg_read_u16(REG_HL, gb);
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
    byte_t reg;

    switch (opcode->opcode) {
        case 0x07: reg = REG_A; break;
        case 0x00: reg = REG_B; break;
        case 0x01: reg = REG_C; break;
        case 0x02: reg = REG_D; break;
        case 0x03: reg = REG_E; break;
        case 0x04: reg = REG_H; break;
        case 0x05: reg = REG_L; break;
        default: return OPCODE_ILLEGAL;
    }

    reg_writeb(reg, cpu_rlc(reg_readb(reg, gb), gb), gb);
    return opcode->cycles_true;
}

// (CB) RL r
int opcode_cb_rl_r(const opcode_t *opcode, gb_system_t *gb)
{
    byte_t reg;

    switch (opcode->opcode) {
        case 0x17: reg = REG_A; break;
        case 0x10: reg = REG_B; break;
        case 0x11: reg = REG_C; break;
        case 0x12: reg = REG_D; break;
        case 0x13: reg = REG_E; break;
        case 0x14: reg = REG_H; break;
        case 0x15: reg = REG_L; break;
        default: return OPCODE_ILLEGAL;
    }

    reg_writeb(reg, cpu_rl(reg_readb(reg, gb), gb), gb);
    return opcode->cycles_true;
}

// (CB) RRC r
int opcode_cb_rrc_r(const opcode_t *opcode, gb_system_t *gb)
{
    byte_t reg;

    switch (opcode->opcode) {
        case 0x0F: reg = REG_A; break;
        case 0x08: reg = REG_B; break;
        case 0x09: reg = REG_C; break;
        case 0x0A: reg = REG_D; break;
        case 0x0B: reg = REG_E; break;
        case 0x0C: reg = REG_H; break;
        case 0x0D: reg = REG_L; break;
        default: return OPCODE_ILLEGAL;
    }

    reg_writeb(reg, cpu_rrc(reg_readb(reg, gb), gb), gb);
    return opcode->cycles_true;
}

// (CB) RR r
int opcode_cb_rr_r(const opcode_t *opcode, gb_system_t *gb)
{
    byte_t reg;

    switch (opcode->opcode) {
        case 0x1F: reg = REG_A; break;
        case 0x18: reg = REG_B; break;
        case 0x19: reg = REG_C; break;
        case 0x1A: reg = REG_D; break;
        case 0x1B: reg = REG_E; break;
        case 0x1C: reg = REG_H; break;
        case 0x1D: reg = REG_L; break;
        default: return OPCODE_ILLEGAL;
    }

    reg_writeb(reg, cpu_rr(reg_readb(reg, gb), gb), gb);
    return opcode->cycles_true;
}