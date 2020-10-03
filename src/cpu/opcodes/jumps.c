/*
jumps.c
JP, JR, CALL, RST, RET(I) opcodes

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

// JP nn and JP cc,nn opcodes
int opcode_jp(const opcode_t *opcode, gb_system_t *gb)
{
    uint16_t addr = cpu_fetch_u16(gb);

    switch (opcode->opcode) {
        // JP nn
        case 0xC3: gb->pc = addr; return opcode->cycles_true;

        // JP NZ,nn
        case 0xC2:
            if (reg_flag(FLAG_Z, gb) == false) {
                gb->pc = addr;
                return opcode->cycles_true;
            } else {
                return opcode->cycles_false;
            }

        // JP Z,nn
        case 0xCA:
            if (reg_flag(FLAG_Z, gb) == true) {
                gb->pc = addr;
                return opcode->cycles_true;
            } else {
                return opcode->cycles_false;
            }

        // JP NC,nn
        case 0xD2:
            if (reg_flag(FLAG_C, gb) == false) {
                gb->pc = addr;
                return opcode->cycles_true;
            } else {
                return opcode->cycles_false;
            }

        // JP C,nn
        case 0xDA:
            if (reg_flag(FLAG_C, gb) == true) {
                gb->pc = addr;
                return opcode->cycles_true;
            } else {
                return opcode->cycles_false;
            }

        default: return OPCODE_ILLEGAL;
    }
}

// JP (HL) opcode
int opcode_jp_hl(const opcode_t *opcode, gb_system_t *gb)
{
    gb->pc = reg_read_u16(REG_HL, gb);

    return opcode->cycles_true;
}

// JR n and JR cc,n opcodes
int opcode_jr(const opcode_t *opcode, gb_system_t *gb)
{
    sbyte_t n = (sbyte_t) cpu_fetchb(gb);
    uint16_t addr = gb->pc + n;

    switch (opcode->opcode) {
        // JR n
        case 0x18: gb->pc = addr; return opcode->cycles_true;

        // JR NZ,n
        case 0x20:
            if (reg_flag(FLAG_Z, gb) == false) {
                gb->pc = addr;
                return opcode->cycles_true;
            } else {
                return opcode->cycles_false;
            }

        // JR Z,n
        case 0x28:
            if (reg_flag(FLAG_Z, gb) == true) {
                gb->pc = addr;
                return opcode->cycles_true;
            } else {
                return opcode->cycles_false;
            }

        // JR NC,n
        case 0x30:
            if (reg_flag(FLAG_C, gb) == false) {
                gb->pc = addr;
                return opcode->cycles_true;
            } else {
                return opcode->cycles_false;
            }

        // JR C,n
        case 0x38:
            if (reg_flag(FLAG_C, gb) == true) {
                gb->pc = addr;
                return opcode->cycles_true;
            } else {
                return opcode->cycles_false;
            }

        default: return OPCODE_ILLEGAL;
    }
}