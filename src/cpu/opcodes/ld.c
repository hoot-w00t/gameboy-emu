/*
ld.c
CPU Stack push/pop functions
LD, PUSH, POP opcodes

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
#include "mmu/mmu.h"
#include "cpu/registers.h"

// Push byte to stack
void cpu_pushb(byte_t value, gb_system_t *gb)
{
    mmu_writeb(gb->sp, value, gb);
    gb->sp -= 1;
}

// Push uint16 to stack
void cpu_push_u16(uint16_t value, gb_system_t *gb)
{
    cpu_pushb(value >> 8, gb);
    cpu_pushb(value & 0xff, gb);
}

// Pop byte from stack
byte_t cpu_popb(gb_system_t *gb)
{
    gb->sp += 1;
    return mmu_readb(gb->sp, gb);
}

// Pop uint16 from stack
uint16_t cpu_pop_u16(gb_system_t *gb)
{
    uint16_t value = mmu_read_u16(gb->sp + 1, gb);

    gb->sp += 2;
    return value;
}

// PUSH opcodes
int opcode_push(const opcode_t *opcode, gb_system_t *gb)
{
    switch (opcode->opcode) {
        case 0xC5: cpu_push_u16(reg_read_u16(REG_BC, gb), gb); break;
        case 0xD5: cpu_push_u16(reg_read_u16(REG_DE, gb), gb); break;
        case 0xE5: cpu_push_u16(reg_read_u16(REG_HL, gb), gb); break;
        case 0xF5: cpu_push_u16(reg_read_u16(REG_AF, gb), gb); break;
        default: return OPCODE_ILLEGAL;
    }
    return opcode->cycles_true;
}

// POP opcodes
int opcode_pop(const opcode_t *opcode, gb_system_t *gb)
{
    switch (opcode->opcode) {
        case 0xC1: reg_write_u16(REG_BC, cpu_pop_u16(gb), gb); break;
        case 0xD1: reg_write_u16(REG_DE, cpu_pop_u16(gb), gb); break;
        case 0xE1: reg_write_u16(REG_HL, cpu_pop_u16(gb), gb); break;
        // TODO: set and clear relevant flags when popping to AF
        case 0xF1: reg_write_u16(REG_AF, cpu_pop_u16(gb), gb); break;
        default: return OPCODE_ILLEGAL;
    }
    return opcode->cycles_true;
}

// Set 8bit dest_reg to src_reg
static inline void ld_rr8(const byte_t dest_reg, const byte_t src_reg, gb_system_t *gb)
{
    reg_writeb(dest_reg, reg_readb(src_reg, gb), gb);
}

// Set 8bit reg to value stored at addr in HL
static inline void ld_r8_hl(const byte_t reg, gb_system_t *gb)
{
    reg_writeb(reg, mmu_readb(reg_read_u16(REG_HL, gb), gb), gb);
}

// Write value of reg at addr in HL
static inline void ld_hl_r8(const byte_t reg, gb_system_t *gb)
{
    mmu_writeb(reg_read_u16(REG_HL, gb), reg_readb(reg, gb), gb);
}

// LD r,r and LD r,n excluding the A register
int opcode_ld_r8(const opcode_t *opcode, gb_system_t *gb)
{
    switch (opcode->opcode) {
        // LD r,n
        case 0x06: reg_writeb(REG_B, cpu_fetchb(gb), gb); break;
        case 0x0E: reg_writeb(REG_C, cpu_fetchb(gb), gb); break;
        case 0x16: reg_writeb(REG_D, cpu_fetchb(gb), gb); break;
        case 0x1E: reg_writeb(REG_E, cpu_fetchb(gb), gb); break;
        case 0x26: reg_writeb(REG_H, cpu_fetchb(gb), gb); break;
        case 0x2E: reg_writeb(REG_L, cpu_fetchb(gb), gb); break;

        // LD B,r + (HL)
        case 0x40: ld_rr8(REG_B, REG_B, gb); break;
        case 0x41: ld_rr8(REG_B, REG_C, gb); break;
        case 0x42: ld_rr8(REG_B, REG_D, gb); break;
        case 0x43: ld_rr8(REG_B, REG_E, gb); break;
        case 0x44: ld_rr8(REG_B, REG_H, gb); break;
        case 0x45: ld_rr8(REG_B, REG_L, gb); break;
        case 0x46: ld_r8_hl(REG_B, gb); break;

        // LD C,r + (HL)
        case 0x48: ld_rr8(REG_C, REG_B, gb); break;
        case 0x49: ld_rr8(REG_C, REG_C, gb); break;
        case 0x4A: ld_rr8(REG_C, REG_D, gb); break;
        case 0x4B: ld_rr8(REG_C, REG_E, gb); break;
        case 0x4C: ld_rr8(REG_C, REG_H, gb); break;
        case 0x4D: ld_rr8(REG_C, REG_L, gb); break;
        case 0x4E: ld_r8_hl(REG_C, gb); break;

        // LD D,r + (HL)
        case 0x50: ld_rr8(REG_D, REG_B, gb); break;
        case 0x51: ld_rr8(REG_D, REG_C, gb); break;
        case 0x52: ld_rr8(REG_D, REG_D, gb); break;
        case 0x53: ld_rr8(REG_D, REG_E, gb); break;
        case 0x54: ld_rr8(REG_D, REG_H, gb); break;
        case 0x55: ld_rr8(REG_D, REG_L, gb); break;
        case 0x56: ld_r8_hl(REG_D, gb); break;

        // LD E,r + (HL)
        case 0x58: ld_rr8(REG_E, REG_B, gb); break;
        case 0x59: ld_rr8(REG_E, REG_C, gb); break;
        case 0x5A: ld_rr8(REG_E, REG_D, gb); break;
        case 0x5B: ld_rr8(REG_E, REG_E, gb); break;
        case 0x5C: ld_rr8(REG_E, REG_H, gb); break;
        case 0x5D: ld_rr8(REG_E, REG_L, gb); break;
        case 0x5E: ld_r8_hl(REG_E, gb); break;

        // LD H,r + (HL)
        case 0x60: ld_rr8(REG_H, REG_B, gb); break;
        case 0x61: ld_rr8(REG_H, REG_C, gb); break;
        case 0x62: ld_rr8(REG_H, REG_D, gb); break;
        case 0x63: ld_rr8(REG_H, REG_E, gb); break;
        case 0x64: ld_rr8(REG_H, REG_H, gb); break;
        case 0x65: ld_rr8(REG_H, REG_L, gb); break;
        case 0x66: ld_r8_hl(REG_H, gb); break;

        // LD L,r + (HL)
        case 0x68: ld_rr8(REG_L, REG_B, gb); break;
        case 0x69: ld_rr8(REG_L, REG_C, gb); break;
        case 0x6A: ld_rr8(REG_L, REG_D, gb); break;
        case 0x6B: ld_rr8(REG_L, REG_E, gb); break;
        case 0x6C: ld_rr8(REG_L, REG_H, gb); break;
        case 0x6D: ld_rr8(REG_L, REG_L, gb); break;
        case 0x6E: ld_r8_hl(REG_L, gb); break;

        // LD (HL),r
        case 0x70: ld_hl_r8(REG_B, gb); break;
        case 0x71: ld_hl_r8(REG_C, gb); break;
        case 0x72: ld_hl_r8(REG_D, gb); break;
        case 0x73: ld_hl_r8(REG_E, gb); break;
        case 0x74: ld_hl_r8(REG_H, gb); break;
        case 0x75: ld_hl_r8(REG_L, gb); break;

        // LD (HL),n
        case 0x36: mmu_writeb(reg_read_u16(REG_HL, gb), cpu_fetchb(gb), gb); break;

        default: return OPCODE_ILLEGAL;
    }
    return opcode->cycles_true;
}

// All LD opcodes related to the A register
int opcode_ld_a(const opcode_t *opcode, gb_system_t *gb)
{
    switch (opcode->opcode) {
        // LD A,A
        case 0x7F: ld_rr8(REG_A, REG_A, gb); break;

        // LD A,r
        case 0x78: ld_rr8(REG_A, REG_B, gb); break;
        case 0x79: ld_rr8(REG_A, REG_C, gb); break;
        case 0x7A: ld_rr8(REG_A, REG_D, gb); break;
        case 0x7B: ld_rr8(REG_A, REG_E, gb); break;
        case 0x7C: ld_rr8(REG_A, REG_H, gb); break;
        case 0x7D: ld_rr8(REG_A, REG_L, gb); break;

        // LD r,A
        case 0x47: ld_rr8(REG_B, REG_A, gb); break;
        case 0x4F: ld_rr8(REG_C, REG_A, gb); break;
        case 0x57: ld_rr8(REG_D, REG_A, gb); break;
        case 0x5F: ld_rr8(REG_E, REG_A, gb); break;
        case 0x67: ld_rr8(REG_H, REG_A, gb); break;
        case 0x6F: ld_rr8(REG_L, REG_A, gb); break;

        // LD A,n
        case 0x3E: reg_writeb(REG_A, cpu_fetchb(gb), gb); break;

        // LD A,(nn)
        case 0x0A: reg_writeb(REG_A, mmu_readb(reg_read_u16(REG_BC, gb), gb), gb); break;
        case 0x1A: reg_writeb(REG_A, mmu_readb(reg_read_u16(REG_DE, gb), gb), gb); break;
        case 0x7E: ld_r8_hl(REG_A, gb); break;
        case 0xFA: reg_writeb(REG_A, mmu_readb(cpu_fetch_u16(gb), gb), gb); break;

        // LD (nn),A
        case 0x02: mmu_writeb(reg_read_u16(REG_BC, gb), reg_readb(REG_A, gb), gb); break;
        case 0x12: mmu_writeb(reg_read_u16(REG_DE, gb), reg_readb(REG_A, gb), gb); break;
        case 0x77: ld_hl_r8(REG_A, gb); break;
        case 0xEA: mmu_writeb(cpu_fetch_u16(gb), reg_readb(REG_A, gb), gb); break;

        // LD A,(C)
        case 0xF2: reg_writeb(REG_A, mmu_readb(0xFF00 + reg_readb(REG_C, gb), gb), gb); break;

        // LD (C),A
        case 0xE2: mmu_writeb(0xFF00 + reg_readb(REG_C, gb), reg_readb(REG_A, gb), gb); break;

        // LDD A,(HL) and LDI A,(HL)
        case 0x3A:
            ld_r8_hl(REG_A, gb);
            reg_write_u16(REG_HL, reg_read_u16(REG_HL, gb) - 1, gb);
            break;
        case 0x2A:
            ld_r8_hl(REG_A, gb);
            reg_write_u16(REG_HL, reg_read_u16(REG_HL, gb) + 1, gb);
            break;

        // LDD (HL),A and LDI (HL),A
        case 0x32:
            ld_hl_r8(REG_A, gb);
            reg_write_u16(REG_HL, reg_read_u16(REG_HL, gb) - 1, gb);
            break;
        case 0x22:
            ld_hl_r8(REG_A, gb);
            reg_write_u16(REG_HL, reg_read_u16(REG_HL, gb) + 1, gb);
            break;

        // LDH (n),A and LDH A,(n)
        case 0xE0: mmu_writeb(0xFF00 + cpu_fetchb(gb), reg_readb(REG_A, gb), gb); break;
        case 0xF0: reg_writeb(REG_A, mmu_readb(0xFF00 + cpu_fetchb(gb), gb), gb); break;

        default: return OPCODE_ILLEGAL;
    }
    return opcode->cycles_true;
}

// LD r16,nn opcodes
int opcode_ld_r16_nn(const opcode_t *opcode, gb_system_t *gb)
{
    switch (opcode->opcode) {
        case 0x01: reg_write_u16(REG_BC, cpu_fetch_u16(gb), gb); break;
        case 0x11: reg_write_u16(REG_DE, cpu_fetch_u16(gb), gb); break;
        case 0x21: reg_write_u16(REG_HL, cpu_fetch_u16(gb), gb); break;
        default: return OPCODE_ILLEGAL;
    }
    return opcode->cycles_true;
}

// All LD opcodes related to the SP
int opcode_ld_sp(const opcode_t *opcode, gb_system_t *gb)
{
    switch (opcode->opcode) {
        // LD SP,nn
        case 0x31: gb->sp = cpu_fetch_u16(gb); break;

        // LD SP,HL
        case 0xF9: gb->sp = reg_read_u16(REG_HL, gb); break;

        // TODO: LD SP,n ($F8), depends on ADD

        // LD (nn),SP
        case 0x08: mmu_write_u16(cpu_fetch_u16(gb), gb->sp, gb); break;

        default: return OPCODE_ILLEGAL;
    }
    return opcode->cycles_true;
}