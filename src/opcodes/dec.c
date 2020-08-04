/*
dec.c
CPU decrement operations

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
#include "logger.h"

// Decrement 8 bit register
void gb_dec_8b_reg(byte_t reg, gb_system_t *gb)
{
    byte_t result = gb_register_read_byte(reg, gb) - 1;

    gb_register_write_byte(reg, result, gb);
    if (result) {
        gb_flag_clear(FLAG_Z, gb);
    } else {
        gb_flag_set(FLAG_Z, gb);
    }
    gb_flag_set(FLAG_N, gb);
    if (result == 0xFF) {
        gb_flag_set(FLAG_H, gb);
    } else {
        gb_flag_clear(FLAG_H, gb);
    }
}

// Decrement 16 bit register
void gb_dec_16b_reg(byte_t reg, gb_system_t *gb)
{
    gb_register_write_u16(reg, gb_register_read_u16(reg, gb) - 1, gb);
}

// Decrement byte in memory
void gb_dec_address(uint16_t address, gb_system_t *gb)
{
    byte_t result = gb_read_byte(address, gb) - 1;

    gb_write_byte(address, result, false, gb);
    if (result) {
        gb_flag_clear(FLAG_Z, gb);
    } else {
        gb_flag_set(FLAG_Z, gb);
    }
    gb_flag_set(FLAG_N, gb);
    if (result == 0xFF) {
        gb_flag_set(FLAG_H, gb);
    } else {
        gb_flag_clear(FLAG_H, gb);
    }
}

// DEC n
int gb_opcode_dec(const gb_opcode_t *opcode, gb_system_t *gb)
{
    switch (opcode->opcode) {
        case 0x05: // DEC B
            gb_dec_8b_reg(REG_B, gb);
            return OPCODE_ACTION;

        case 0x0B: // DEC BC
            gb_dec_16b_reg(REG_BC, gb);
            return OPCODE_ACTION;

        case 0x0D: // DEC C
            gb_dec_8b_reg(REG_C, gb);
            return OPCODE_ACTION;

        case 0x15: // DEC D
            gb_dec_8b_reg(REG_D, gb);
            return OPCODE_ACTION;

        case 0x1B: // DEC DE
            gb_dec_16b_reg(REG_DE, gb);
            return OPCODE_ACTION;

        case 0x1D: // DEC E
            gb_dec_8b_reg(REG_E, gb);
            return OPCODE_ACTION;

        case 0x25: // DEC H
            gb_dec_8b_reg(REG_H, gb);
            return OPCODE_ACTION;

        case 0x2B: // DEC HL
            gb_dec_16b_reg(REG_HL, gb);
            return OPCODE_ACTION;

        case 0x2D: // DEC L
            gb_dec_8b_reg(REG_L, gb);
            return OPCODE_ACTION;

        case 0x35: // DEC (HL)
            gb_dec_address(gb_register_read_u16(REG_HL, gb), gb);
            return OPCODE_ACTION;

        case 0x3B: // DEC SP
            gb->sp -= 1;
            return OPCODE_ACTION;

        case 0x3D: // DEC A
            gb_dec_8b_reg(REG_A, gb);
            return OPCODE_ACTION;

        default:
            return OPCODE_ILLEGAL;
    }
}