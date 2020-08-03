/*
and.c
CPU Logical AND operations

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

// Logical AND with register A and given value
// Result stored in register A
// Clear flags N, C, set flag H, and set Z if result is 0
void gb_8b_and(byte_t value, gb_system_t *gb)
{
    byte_t reg_a_value = gb_register_read_byte(REG_A, gb);
    byte_t result = reg_a_value & value;

    gb_register_write_byte(REG_A, result, gb);
    if (result) {
        gb_flag_set(FLAG_Z, gb);
    } else {
        gb_flag_clear(FLAG_Z, gb);
    }
    gb_flag_clear(FLAG_N, gb);
    gb_flag_set(FLAG_H, gb);
    gb_flag_clear(FLAG_C, gb);
}

// AND N
int gb_opcode_and(const gb_opcode_t *opcode, gb_system_t *gb)
{
    switch (opcode->opcode) {
        case 0xA0:
            gb_8b_and(gb_register_read_byte(REG_B, gb), gb);
            return OPCODE_ACTION;
        case 0xA1:
            gb_8b_and(gb_register_read_byte(REG_C, gb), gb);
            return OPCODE_ACTION;
        case 0xA2:
            gb_8b_and(gb_register_read_byte(REG_D, gb), gb);
            return OPCODE_ACTION;
        case 0xA3:
            gb_8b_and(gb_register_read_byte(REG_E, gb), gb);
            return OPCODE_ACTION;
        case 0xA4:
            gb_8b_and(gb_register_read_byte(REG_H, gb), gb);
            return OPCODE_ACTION;
        case 0xA5:
            gb_8b_and(gb_register_read_byte(REG_L, gb), gb);
            return OPCODE_ACTION;
        case 0xA6:
            gb_8b_and(gb_register_read_u16(REG_HL, gb), gb); // Is it HL or the value stored at address HL?
            return OPCODE_ACTION;
        case 0xA7:
            gb_8b_and(gb_register_read_byte(REG_A, gb), gb);
            return OPCODE_ACTION;
        case 0xE6:
            gb_8b_and(gb_read_byte(gb->pc + 1, gb), gb);
            return OPCODE_ACTION;
        default:
            return OPCODE_ILLEGAL;
    }
}