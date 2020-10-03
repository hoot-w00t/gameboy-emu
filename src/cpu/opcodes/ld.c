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