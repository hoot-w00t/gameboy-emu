/*
cp.c
CPU Compare operations

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

// Compare value with register A (subtract value from A)
// Set Zero Flag if result is 0 (A == n)
// Set N
// Set H if no borrow from bit 4
// Set C if no borrow (A < n)
void gb_cp(byte_t value, gb_system_t *gb)
{
    byte_t A = gb_register_read_byte(REG_A, gb);
    byte_t result = A - value;

    if (result) {
        gb_flag_clear(FLAG_Z, gb);
    } else {
        gb_flag_set(FLAG_Z, gb);
    }
    gb_flag_set(FLAG_N, gb);
    if (value < 0xF) {
        gb_flag_set(FLAG_H, gb);
    } else {
        gb_flag_clear(FLAG_H, gb);
    }
    if (A < value) {
        gb_flag_set(FLAG_C, gb);
    } else {
        gb_flag_clear(FLAG_C, gb);
    }
}

// CP instruction
int gb_opcode_cp(const gb_opcode_t *opcode, gb_system_t *gb)
{
    switch (opcode->opcode) {
        case 0xB8: // CP B
            gb_cp(gb_register_read_byte(REG_B, gb), gb);
            return OPCODE_ACTION;

        case 0xB9: // CP C
            gb_cp(gb_register_read_byte(REG_C, gb), gb);
            return OPCODE_ACTION;

        case 0xBA: // CP D
            gb_cp(gb_register_read_byte(REG_D, gb), gb);
            return OPCODE_ACTION;

        case 0xBB: // CP E
            gb_cp(gb_register_read_byte(REG_E, gb), gb);
            return OPCODE_ACTION;

        case 0xBC: // CP H
            gb_cp(gb_register_read_byte(REG_H, gb), gb);
            return OPCODE_ACTION;

        case 0xBD: // CP L
            gb_cp(gb_register_read_byte(REG_L, gb), gb);
            return OPCODE_ACTION;

        case 0xBE: // CP (HL)
            gb_cp(gb_read_byte(gb_register_read_u16(REG_HL, gb), gb), gb);
            return OPCODE_ACTION;

        case 0xBF: // CP A
            gb_cp(gb_register_read_byte(REG_A, gb), gb);
            return OPCODE_ACTION;

        case 0xFE: // CP d8
            gb_cp(gb_read_byte(gb->pc + 1, gb), gb);
            return OPCODE_ACTION;

        default:
            return OPCODE_ILLEGAL;
    }
}