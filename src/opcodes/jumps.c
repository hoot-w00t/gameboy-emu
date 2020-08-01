/*
jumps.c
CPU Jump/Call/Return operations

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

#include "gb/opcodes.h"
#include "gb/memory.h"
#include "gb/registers.h"
#include "gb/cpu.h"
#include "logger.h"

// JP instruction
int gb_opcode_jp(const gb_opcode_t *opcode, gb_system_t *gb)
{
    switch (opcode->opcode) {
        case 0xC2:
            if (!gb_flag(FLAG_Z, gb)) { // NZ
                gb->pc = gb_read_uint16(gb->pc + 1, gb);
                return OPCODE_ACTION | OPCODE_NOPC;
            } else {
                return OPCODE_NOACTION;
            }

        case 0xC3:
            gb->pc = gb_read_uint16(gb->pc + 1, gb);
            return OPCODE_ACTION | OPCODE_NOPC;

        case 0xCA:
            if (gb_flag(FLAG_Z, gb)) { // Z
                gb->pc = gb_read_uint16(gb->pc + 1, gb);
                return OPCODE_ACTION | OPCODE_NOPC;
            } else {
                return OPCODE_NOACTION;
            }

        case 0xD2:
            if (!gb_flag(FLAG_C, gb)) { // NC
                gb->pc = gb_read_uint16(gb->pc + 1, gb);
                return OPCODE_ACTION | OPCODE_NOPC;
            } else {
                return OPCODE_NOACTION;
            }

        case 0xDA:
            if (gb_flag(FLAG_C, gb)) { // C
                gb->pc = gb_read_uint16(gb->pc + 1, gb);
                return OPCODE_ACTION | OPCODE_NOPC;
            } else {
                return OPCODE_NOACTION;
            }

        case 0xE9:
            gb->pc = gb_register_read_u16(REG_HL, gb);
            return OPCODE_ACTION | OPCODE_NOPC;

        default:
            return OPCODE_ILLEGAL;
    }
}

// JR instruction
int gb_opcode_jr(const gb_opcode_t *opcode, gb_system_t *gb)
{
    switch (opcode->opcode) {
        case 0x18:
            gb->pc += (sbyte_t) gb_read_byte(gb->pc + 1, gb);
            return OPCODE_ACTION | OPCODE_NOPC;

        case 0x20:
            if (!gb_flag(FLAG_Z, gb)) {
                gb->pc += (sbyte_t) gb_read_byte(gb->pc + 1, gb);
                return OPCODE_ACTION | OPCODE_NOPC;
            } else {
                return OPCODE_NOACTION;
            }

        case 0x28:
            if (gb_flag(FLAG_Z, gb)) {
                gb->pc += (sbyte_t) gb_read_byte(gb->pc + 1, gb);
                return OPCODE_ACTION | OPCODE_NOPC;
            } else {
                return OPCODE_NOACTION;
            }

        case 0x30:
            if (!gb_flag(FLAG_C, gb)) {
                gb->pc += (sbyte_t) gb_read_byte(gb->pc + 1, gb);
                return OPCODE_ACTION | OPCODE_NOPC;
            } else {
                return OPCODE_NOACTION;
            }

        case 0x38:
            if (gb_flag(FLAG_C, gb)) {
                gb->pc += (sbyte_t) gb_read_byte(gb->pc + 1, gb);
                return OPCODE_ACTION | OPCODE_NOPC;
            } else {
                return OPCODE_NOACTION;
            }

        default:
            return OPCODE_ILLEGAL;
    }
}

// CALL instruction
int gb_opcode_call(const gb_opcode_t *opcode, gb_system_t *gb)
{
    uint16_t address = gb_read_uint16(gb->pc + 1, gb);

    switch (opcode->opcode) {
        case 0xC4:
            if (!gb_flag(FLAG_Z, gb)) {
                gb_cpu_call(address, gb);
                return OPCODE_ACTION | OPCODE_NOPC;
            } else {
                return OPCODE_NOACTION;
            }

        case 0xCC:
            if (gb_flag(FLAG_Z, gb)) {
                gb_cpu_call(address, gb);
                return OPCODE_ACTION | OPCODE_NOPC;
            } else {
                return OPCODE_NOACTION;
            }

        case 0xCD:
            gb_cpu_call(address, gb);
            return OPCODE_ACTION | OPCODE_NOPC;

        case 0xD4:
            if (!gb_flag(FLAG_C, gb)) {
                gb_cpu_call(address, gb);
                return OPCODE_ACTION | OPCODE_NOPC;
            } else {
                return OPCODE_NOACTION;
            }

        case 0xDC:
            if (gb_flag(FLAG_C, gb)) {
                gb_cpu_call(address, gb);
                return OPCODE_ACTION | OPCODE_NOPC;
            } else {
                return OPCODE_NOACTION;
            }

        default:
            return OPCODE_ILLEGAL;
    }
}