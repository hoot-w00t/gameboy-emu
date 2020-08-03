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

// LD SP,HL instruction
int gb_opcode_ld_sp_hl(__attribute__((unused)) const gb_opcode_t *opcode, gb_system_t *gb)
{
    gb->sp = gb_register_read_u16(REG_HL, gb);

    return OPCODE_ACTION;
}

// LD SP,HL+r8 instruction
int gb_opcode_ld_hl_sp_r8(__attribute__((unused)) const gb_opcode_t *opcode, gb_system_t *gb)
{
    sbyte_t r8 = (sbyte_t) gb_read_byte(gb->pc + 1, gb);
    uint16_t value = gb->sp + r8;

    gb_register_write_u16(REG_HL, value, gb);
    gb_flag_clear(FLAG_Z, gb);
    gb_flag_clear(FLAG_N, gb);
    gb_flag_n(r8 < 0, gb);
    if ((r8 < 0 && value > gb->sp) || (r8 >= 0 && value < gb->sp)) {
        gb_flag_set(FLAG_H, gb);
    }

    return OPCODE_ACTION;
}

// LD r,d8
int gb_opcode_ld_r_d8(const gb_opcode_t *opcode, gb_system_t *gb)
{
    switch (opcode->opcode) {
        case 0x06:
            gb_register_write_byte(REG_B, gb_read_byte(gb->pc + 1, gb), gb);
            return OPCODE_ACTION;
        case 0x0E:
            gb_register_write_byte(REG_C, gb_read_byte(gb->pc + 1, gb), gb);
            return OPCODE_ACTION;
        case 0x16:
            gb_register_write_byte(REG_D, gb_read_byte(gb->pc + 1, gb), gb);
            return OPCODE_ACTION;
        case 0x1E:
            gb_register_write_byte(REG_E, gb_read_byte(gb->pc + 1, gb), gb);
            return OPCODE_ACTION;
        case 0x26:
            gb_register_write_byte(REG_H, gb_read_byte(gb->pc + 1, gb), gb);
            return OPCODE_ACTION;
        case 0x2E:
            gb_register_write_byte(REG_L, gb_read_byte(gb->pc + 1, gb), gb);
            return OPCODE_ACTION;
        case 0x36:
            gb_write_byte(gb_register_read_u16(REG_HL, gb), gb_read_byte(gb->pc + 1, gb), false, gb);
            return OPCODE_ACTION;
        case 0x3E:
            gb_register_write_byte(REG_A, gb_read_byte(gb->pc + 1, gb), gb);
            return OPCODE_ACTION;
        default:
            return OPCODE_ACTION;
    }
}

// LDI (HL),A / LDI A,(HL)
int gb_opcode_ldi(const gb_opcode_t *opcode, gb_system_t *gb)
{
    uint16_t hl = gb_register_read_u16(REG_HL, gb);

    switch (opcode->opcode) {
        case 0x22:
            gb_write_byte(hl, gb_register_read_byte(REG_A, gb), false, gb);
            hl += 1;
            gb_register_write_u16(REG_HL, hl, gb);
            return OPCODE_ACTION;
        case 0x2A:
            gb_register_write_byte(REG_A, gb_read_byte(hl, gb), gb);
            hl += 1;
            gb_register_write_u16(REG_HL, hl, gb);
            return OPCODE_ACTION;
        default:
            return OPCODE_ILLEGAL;
    }
}

// LDD (HL),A / LDD A,(HL)
int gb_opcode_ldd(const gb_opcode_t *opcode, gb_system_t *gb)
{
    uint16_t hl = gb_register_read_u16(REG_HL, gb);

    switch (opcode->opcode) {
        case 0x32:
            gb_write_byte(hl, gb_register_read_byte(REG_A, gb), false, gb);
            hl -= 1;
            gb_register_write_u16(REG_HL, hl, gb);
            return OPCODE_ACTION;
        case 0x3A:
            gb_register_write_byte(REG_A, gb_read_byte(hl, gb), gb);
            hl -= 1;
            gb_register_write_u16(REG_HL, hl, gb);
            return OPCODE_ACTION;
        default:
            return OPCODE_ILLEGAL;
    }
}