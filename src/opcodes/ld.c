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

// LDH d8,A / LDH A,d8
int gb_opcode_ldh(const gb_opcode_t *opcode, gb_system_t *gb)
{
    switch (opcode->opcode) {
        case 0xE0:
            gb_write_byte(
                0xFF00 + gb_read_byte(gb->pc + 1, gb),
                gb_register_read_byte(REG_A, gb),
                false,
                gb
            );
            return OPCODE_ACTION;

        case 0xF0:
            gb_register_write_byte(
                REG_A,
                gb_read_byte(0xFF00 + gb_read_byte(gb->pc + 1, gb), gb),
                gb
            );
            return OPCODE_ACTION;

        default:
            return OPCODE_ILLEGAL;
    }
}

// LD r,r or LD r,(a16) or LD (a16),r
// LD operations for r,r  r,(rr)  (rr),r
int gb_opcode_ld_r_r(const gb_opcode_t *opcode, gb_system_t *gb)
{
    switch (opcode->opcode) {
        case 0x02:
            gb_write_byte(gb_register_read_u16(REG_BC, gb), gb_register_read_byte(REG_A, gb), false, gb);
            return OPCODE_ACTION;

        case 0x0A:
            gb_register_write_byte(REG_A, gb_read_byte(gb_register_read_u16(REG_BC, gb), gb), gb);
            return OPCODE_ACTION;

        case 0x12:
            gb_write_byte(gb_register_read_u16(REG_DE, gb), gb_register_read_byte(REG_A, gb), false, gb);
            return OPCODE_ACTION;

        case 0x1A:
            gb_register_write_byte(REG_A, gb_read_byte(gb_register_read_u16(REG_DE, gb), gb), gb);
            return OPCODE_ACTION;

        case 0x40:
            gb_register_write_byte(REG_B, gb_register_read_byte(REG_B, gb), gb);
            return OPCODE_ACTION;

        case 0x41:
            gb_register_write_byte(REG_B, gb_register_read_byte(REG_C, gb), gb);
            return OPCODE_ACTION;

        case 0x42:
            gb_register_write_byte(REG_B, gb_register_read_byte(REG_D, gb), gb);
            return OPCODE_ACTION;

        case 0x43:
            gb_register_write_byte(REG_B, gb_register_read_byte(REG_E, gb), gb);
            return OPCODE_ACTION;

        case 0x44:
            gb_register_write_byte(REG_B, gb_register_read_byte(REG_H, gb), gb);
            return OPCODE_ACTION;

        case 0x45:
            gb_register_write_byte(REG_B, gb_register_read_byte(REG_L, gb), gb);
            return OPCODE_ACTION;

        case 0x46:
            gb_register_write_byte(REG_B, gb_read_byte(gb_register_read_u16(REG_HL, gb), gb), gb);
            return OPCODE_ACTION;

        case 0x47:
            gb_register_write_byte(REG_B, gb_register_read_byte(REG_A, gb), gb);
            return OPCODE_ACTION;

        case 0x48:
            gb_register_write_byte(REG_C, gb_register_read_byte(REG_B, gb), gb);
            return OPCODE_ACTION;

        case 0x49:
            gb_register_write_byte(REG_C, gb_register_read_byte(REG_C, gb), gb);
            return OPCODE_ACTION;

        case 0x4A:
            gb_register_write_byte(REG_C, gb_register_read_byte(REG_D, gb), gb);
            return OPCODE_ACTION;

        case 0x4B:
            gb_register_write_byte(REG_C, gb_register_read_byte(REG_E, gb), gb);
            return OPCODE_ACTION;

        case 0x4C:
            gb_register_write_byte(REG_C, gb_register_read_byte(REG_H, gb), gb);
            return OPCODE_ACTION;

        case 0x4D:
            gb_register_write_byte(REG_C, gb_register_read_byte(REG_L, gb), gb);
            return OPCODE_ACTION;

        case 0x4E:
            gb_register_write_byte(REG_C, gb_read_byte(gb_register_read_u16(REG_HL, gb), gb), gb);
            return OPCODE_ACTION;

        case 0x4F:
            gb_register_write_byte(REG_C, gb_register_read_byte(REG_A, gb), gb);
            return OPCODE_ACTION;

        case 0x50:
            gb_register_write_byte(REG_D, gb_register_read_byte(REG_B, gb), gb);
            return OPCODE_ACTION;

        case 0x51:
            gb_register_write_byte(REG_D, gb_register_read_byte(REG_C, gb), gb);
            return OPCODE_ACTION;

        case 0x52:
            gb_register_write_byte(REG_D, gb_register_read_byte(REG_D, gb), gb);
            return OPCODE_ACTION;

        case 0x53:
            gb_register_write_byte(REG_D, gb_register_read_byte(REG_E, gb), gb);
            return OPCODE_ACTION;

        case 0x54:
            gb_register_write_byte(REG_D, gb_register_read_byte(REG_H, gb), gb);
            return OPCODE_ACTION;

        case 0x55:
            gb_register_write_byte(REG_D, gb_register_read_byte(REG_L, gb), gb);
            return OPCODE_ACTION;

        case 0x56:
            gb_register_write_byte(REG_D, gb_read_byte(gb_register_read_u16(REG_HL, gb), gb), gb);
            return OPCODE_ACTION;

        case 0x57:
            gb_register_write_byte(REG_D, gb_register_read_byte(REG_A, gb), gb);
            return OPCODE_ACTION;

        case 0x58:
            gb_register_write_byte(REG_E, gb_register_read_byte(REG_B, gb), gb);
            return OPCODE_ACTION;

        case 0x59:
            gb_register_write_byte(REG_E, gb_register_read_byte(REG_C, gb), gb);
            return OPCODE_ACTION;

        case 0x5A:
            gb_register_write_byte(REG_E, gb_register_read_byte(REG_D, gb), gb);
            return OPCODE_ACTION;

        case 0x5B:
            gb_register_write_byte(REG_E, gb_register_read_byte(REG_E, gb), gb);
            return OPCODE_ACTION;

        case 0x5C:
            gb_register_write_byte(REG_E, gb_register_read_byte(REG_H, gb), gb);
            return OPCODE_ACTION;

        case 0x5D:
            gb_register_write_byte(REG_E, gb_register_read_byte(REG_L, gb), gb);
            return OPCODE_ACTION;

        case 0x5E:
            gb_register_write_byte(REG_E, gb_read_byte(gb_register_read_u16(REG_HL, gb), gb), gb);
            return OPCODE_ACTION;

        case 0x5F:
            gb_register_write_byte(REG_E, gb_register_read_byte(REG_A, gb), gb);
            return OPCODE_ACTION;

        case 0x60:
            gb_register_write_byte(REG_H, gb_register_read_byte(REG_B, gb), gb);
            return OPCODE_ACTION;

        case 0x61:
            gb_register_write_byte(REG_H, gb_register_read_byte(REG_C, gb), gb);
            return OPCODE_ACTION;

        case 0x62:
            gb_register_write_byte(REG_H, gb_register_read_byte(REG_D, gb), gb);
            return OPCODE_ACTION;

        case 0x63:
            gb_register_write_byte(REG_H, gb_register_read_byte(REG_E, gb), gb);
            return OPCODE_ACTION;

        case 0x64:
            gb_register_write_byte(REG_H, gb_register_read_byte(REG_H, gb), gb);
            return OPCODE_ACTION;

        case 0x65:
            gb_register_write_byte(REG_H, gb_register_read_byte(REG_L, gb), gb);
            return OPCODE_ACTION;

        case 0x66:
            gb_register_write_byte(REG_H, gb_read_byte(gb_register_read_u16(REG_HL, gb), gb), gb);
            return OPCODE_ACTION;

        case 0x67:
            gb_register_write_byte(REG_H, gb_register_read_byte(REG_A, gb), gb);
            return OPCODE_ACTION;

        case 0x68:
            gb_register_write_byte(REG_L, gb_register_read_byte(REG_B, gb), gb);
            return OPCODE_ACTION;

        case 0x69:
            gb_register_write_byte(REG_L, gb_register_read_byte(REG_C, gb), gb);
            return OPCODE_ACTION;

        case 0x6A:
            gb_register_write_byte(REG_L, gb_register_read_byte(REG_D, gb), gb);
            return OPCODE_ACTION;

        case 0x6B:
            gb_register_write_byte(REG_L, gb_register_read_byte(REG_E, gb), gb);
            return OPCODE_ACTION;

        case 0x6C:
            gb_register_write_byte(REG_L, gb_register_read_byte(REG_H, gb), gb);
            return OPCODE_ACTION;

        case 0x6D:
            gb_register_write_byte(REG_L, gb_register_read_byte(REG_L, gb), gb);
            return OPCODE_ACTION;

        case 0x6E:
            gb_register_write_byte(REG_L, gb_read_byte(gb_register_read_u16(REG_HL, gb), gb), gb);
            return OPCODE_ACTION;

        case 0x6F:
            gb_register_write_byte(REG_L, gb_register_read_byte(REG_A, gb), gb);
            return OPCODE_ACTION;

        case 0x70:
            gb_write_byte(gb_register_read_u16(REG_HL, gb), gb_register_read_byte(REG_B, gb), false, gb);
            return OPCODE_ACTION;

        case 0x71:
            gb_write_byte(gb_register_read_u16(REG_HL, gb), gb_register_read_byte(REG_C, gb), false, gb);
            return OPCODE_ACTION;

        case 0x72:
            gb_write_byte(gb_register_read_u16(REG_HL, gb), gb_register_read_byte(REG_D, gb), false, gb);
            return OPCODE_ACTION;

        case 0x73:
            gb_write_byte(gb_register_read_u16(REG_HL, gb), gb_register_read_byte(REG_E, gb), false, gb);
            return OPCODE_ACTION;

        case 0x74:
            gb_write_byte(gb_register_read_u16(REG_HL, gb), gb_register_read_byte(REG_H, gb), false, gb);
            return OPCODE_ACTION;

        case 0x75:
            gb_write_byte(gb_register_read_u16(REG_HL, gb), gb_register_read_byte(REG_L, gb), false, gb);
            return OPCODE_ACTION;

        case 0x77:
            gb_write_byte(gb_register_read_u16(REG_HL, gb), gb_register_read_byte(REG_A, gb), false, gb);
            return OPCODE_ACTION;

        case 0x78:
            gb_register_write_byte(REG_A, gb_register_read_byte(REG_B, gb), gb);
            return OPCODE_ACTION;

        case 0x79:
            gb_register_write_byte(REG_A, gb_register_read_byte(REG_C, gb), gb);
            return OPCODE_ACTION;

        case 0x7A:
            gb_register_write_byte(REG_A, gb_register_read_byte(REG_D, gb), gb);
            return OPCODE_ACTION;

        case 0x7B:
            gb_register_write_byte(REG_A, gb_register_read_byte(REG_E, gb), gb);
            return OPCODE_ACTION;

        case 0x7C:
            gb_register_write_byte(REG_A, gb_register_read_byte(REG_H, gb), gb);
            return OPCODE_ACTION;

        case 0x7D:
            gb_register_write_byte(REG_A, gb_register_read_byte(REG_L, gb), gb);
            return OPCODE_ACTION;

        case 0x7E:
            gb_register_write_byte(REG_A, gb_read_byte(gb_register_read_u16(REG_HL, gb), gb), gb);
            return OPCODE_ACTION;

        case 0x7F:
            gb_register_write_byte(REG_A, gb_register_read_byte(REG_A, gb), gb);
            return OPCODE_ACTION;

        case 0xEA:
            gb_write_byte(gb_read_uint16(gb->pc + 1, gb), gb_register_read_byte(REG_A, gb), false, gb);
            return OPCODE_ACTION;

        case 0xFA:
            gb_register_write_byte(REG_A, gb_read_byte(gb_read_uint16(gb->pc + 1, gb), gb), gb);
            return OPCODE_ACTION;

        default:
            return OPCODE_ILLEGAL;
    }
}

// LD A,(C)
int gb_opcode_ld_a_c(__attribute__((unused)) const gb_opcode_t *opcode, gb_system_t *gb)
{
    uint16_t address = gb_read_byte(0xFF00, gb) + gb_register_read_byte(REG_C, gb);

    gb_register_write_byte(REG_A, gb_read_byte(address, gb), gb);
    return OPCODE_ACTION;
}

// LD (C),A
int gb_opcode_ld_c_a(__attribute__((unused)) const gb_opcode_t *opcode, gb_system_t *gb)
{
    uint16_t address = gb_read_byte(0xFF00, gb) + gb_register_read_byte(REG_C, gb);

    gb_write_byte(address, gb_register_read_byte(REG_A, gb), false, gb);
    return OPCODE_ACTION;
}