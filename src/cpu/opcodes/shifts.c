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
    byte_t result = target << 1;

    if (result == 0) reg_flag_set(FLAG_Z, gb); else reg_flag_clear(FLAG_Z, gb);
    reg_flag_clear(FLAG_N, gb);
    reg_flag_clear(FLAG_H, gb);
    if (target & 0x80) reg_flag_set(FLAG_C, gb); else reg_flag_clear(FLAG_C, gb);

    return result;
}

// If keep_msb is true, bit 7 doesn't change
static inline byte_t cpu_shift_right(const byte_t target, const bool keep_msb, gb_system_t *gb)
{
    byte_t result = target >> 1;

    if (keep_msb && (target & 0x80)) result |= 0x80;

    if (result == 0) reg_flag_set(FLAG_Z, gb); else reg_flag_clear(FLAG_Z, gb);
    reg_flag_clear(FLAG_N, gb);
    reg_flag_clear(FLAG_H, gb);
    if (target & 1) reg_flag_set(FLAG_C, gb); else reg_flag_clear(FLAG_C, gb);

    return result;
}

// SLA/SRA/SRL (HL)
int opcode_cb_shift_hl(const opcode_t *opcode, gb_system_t *gb)
{
    uint16_t addr = reg_read_u16(REG_HL, gb);
    byte_t result;

    switch (opcode->opcode) {
        // SLA (HL)
        case 0x26: result = cpu_shift_left(mmu_readb(addr, gb), gb); break;

        // SRA (HL)
        case 0x2E: result = cpu_shift_right(mmu_readb(addr, gb), true, gb); break;

        // SRL (HL)
        case 0x3E: result = cpu_shift_right(mmu_readb(addr, gb), false, gb); break;

        default: return OPCODE_ILLEGAL;
    }

    mmu_writeb(addr, result, gb);
    return opcode->cycles_true;
}

// SLA r
int opcode_cb_sla_r(const opcode_t *opcode, gb_system_t *gb)
{
    byte_t reg;

    switch (opcode->opcode) {
        case 0x27: reg = REG_A; break;
        case 0x20: reg = REG_B; break;
        case 0x21: reg = REG_C; break;
        case 0x22: reg = REG_D; break;
        case 0x23: reg = REG_E; break;
        case 0x24: reg = REG_H; break;
        case 0x25: reg = REG_L; break;
        default: return OPCODE_ILLEGAL;
    }

    reg_writeb(reg, cpu_shift_left(reg_readb(reg, gb), gb), gb);
    return opcode->cycles_true;
}

// SRA r
int opcode_cb_sra_r(const opcode_t *opcode, gb_system_t *gb)
{
    byte_t reg;

    switch (opcode->opcode) {
        case 0x2F: reg = REG_A; break;
        case 0x28: reg = REG_B; break;
        case 0x29: reg = REG_C; break;
        case 0x2A: reg = REG_D; break;
        case 0x2B: reg = REG_E; break;
        case 0x2C: reg = REG_H; break;
        case 0x2D: reg = REG_L; break;
        default: return OPCODE_ILLEGAL;
    }

    reg_writeb(reg, cpu_shift_right(reg_readb(reg, gb), true, gb), gb);
    return opcode->cycles_true;
}

// SRL r
int opcode_cb_srl_r(const opcode_t *opcode, gb_system_t *gb)
{
    byte_t reg;

    switch (opcode->opcode) {
        case 0x3F: reg = REG_A; break;
        case 0x38: reg = REG_B; break;
        case 0x39: reg = REG_C; break;
        case 0x3A: reg = REG_D; break;
        case 0x3B: reg = REG_E; break;
        case 0x3C: reg = REG_H; break;
        case 0x3D: reg = REG_L; break;
        default: return OPCODE_ILLEGAL;
    }

    reg_writeb(reg, cpu_shift_right(reg_readb(reg, gb), false, gb), gb);
    return opcode->cycles_true;
}