/*
calls.c
CALL, RST and RET(I) opcodes

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
#include "cpu/opcodes/ld.h"

// Call addr
void cpu_call(uint16_t addr, gb_system_t *gb)
{
    cpu_push_u16(gb->pc, gb);
    gb->pc = addr;
}

// Return from call
void cpu_ret(gb_system_t *gb)
{
    gb->pc = cpu_pop_u16(gb);
}

// CALL nn and CALL cc,nn opcodes
int opcode_call(const opcode_t *opcode, gb_system_t *gb)
{
    uint16_t addr = cpu_fetch_u16(gb);

    switch (opcode->opcode) {
        // CALL nn
        case 0xCD: cpu_call(addr, gb); return opcode->cycles_true;

        // CALL NZ,nn
        case 0xC4:
            if (reg_flag(FLAG_Z, gb) == false) {
                cpu_call(addr, gb);
                return opcode->cycles_true;
            } else {
                return opcode->cycles_false;
            }

        // CALL Z,nn
        case 0xCC:
            if (reg_flag(FLAG_Z, gb) == true) {
                cpu_call(addr, gb);
                return opcode->cycles_true;
            } else {
                return opcode->cycles_false;
            }

        // CALL NC,nn
        case 0xD4:
            if (reg_flag(FLAG_C, gb) == false) {
                cpu_call(addr, gb);
                return opcode->cycles_true;
            } else {
                return opcode->cycles_false;
            }

        // CALL C,nn
        case 0xDC:
            if (reg_flag(FLAG_C, gb) == true) {
                cpu_call(addr, gb);
                return opcode->cycles_true;
            } else {
                return opcode->cycles_false;
            }

        default: return OPCODE_ILLEGAL;
    }
}

// RST opcodes
int opcode_rst(const opcode_t *opcode, gb_system_t *gb)
{
    uint16_t addr;

    switch (opcode->opcode) {
        case 0xC7: addr = 0x00; break;
        case 0xCF: addr = 0x08; break;
        case 0xD7: addr = 0x10; break;
        case 0xDF: addr = 0x18; break;
        case 0xE7: addr = 0x20; break;
        case 0xEF: addr = 0x28; break;
        case 0xF7: addr = 0x30; break;
        case 0xFF: addr = 0x38; break;
        default: return OPCODE_ILLEGAL;
    }

    cpu_call(addr, gb);
    return opcode->cycles_true;
}

// RET and RET cc opcodes
int opcode_ret(const opcode_t *opcode, gb_system_t *gb)
{
    switch (opcode->opcode) {
        // RET
        case 0xC9: cpu_ret(gb); return opcode->cycles_true;

        // RET NZ
        case 0xC0:
            if (reg_flag(FLAG_Z, gb) == false) {
                cpu_ret(gb);
                return opcode->cycles_true;
            } else {
                return opcode->cycles_false;
            }

        // RET Z
        case 0xC8:
            if (reg_flag(FLAG_Z, gb) == true) {
                cpu_ret(gb);
                return opcode->cycles_true;
            } else {
                return opcode->cycles_false;
            }

        // RET NC
        case 0xD0:
            if (reg_flag(FLAG_C, gb) == false) {
                cpu_ret(gb);
                return opcode->cycles_true;
            } else {
                return opcode->cycles_false;
            }

        // RET C
        case 0xD8:
            if (reg_flag(FLAG_C, gb) == true) {
                cpu_ret(gb);
                return opcode->cycles_true;
            } else {
                return opcode->cycles_false;
            }

        default: return OPCODE_ILLEGAL;
    }
}

// RETI
int opcode_reti(const opcode_t *opcode, gb_system_t *gb)
{
    cpu_ret(gb);
    gb->interrupts.ime = IME_ENABLE;

    return opcode->cycles_true;
}