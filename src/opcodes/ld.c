/*
ld.c
CPU Load operations

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

// LD RR,d16 instruction
int gb_opcode_ld_rr_d16(const gb_opcode_t *opcode, gb_system_t *gb)
{
    switch (opcode->opcode) {
        case 0x01:
            gb_register_write_u16(REG_BC, gb_read_uint16(gb->pc + 1, gb), gb);
            return OPCODE_ACTION;

        case 0x11:
            gb_register_write_u16(REG_DE, gb_read_uint16(gb->pc + 1, gb), gb);
            return OPCODE_ACTION;

        case 0x21:
            gb_register_write_u16(REG_HL, gb_read_uint16(gb->pc + 1, gb), gb);
            return OPCODE_ACTION;

        case 0x31:
            gb->sp = gb_read_uint16(gb->pc + 1, gb);
            return OPCODE_ACTION;

        default:
            return OPCODE_ILLEGAL;
    }
}