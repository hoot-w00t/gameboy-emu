/*
registers.c
Interaction with CPU registers

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

#include "gb/defs.h"

// Read byte from a CPU register
byte_t gb_register_read_byte(byte_t reg, gb_system_t *gb)
{
    return gb->registers[reg];
}

// Write byte to a CPU register
void gb_register_write_byte(byte_t reg, byte_t value, gb_system_t *gb)
{
    gb->registers[reg] = value;
}

// Read 2 bytes from a 16 bit CPU register
uint16_t gb_register_read_u16(byte_t reg, gb_system_t *gb)
{
    return (gb->registers[reg] << 8) | gb->registers[reg + 1];
}

// Write 2 bytes to a 16 bit CPU register
void gb_register_write_u16(byte_t reg, uint16_t value, gb_system_t *gb)
{
    gb->registers[reg] = value << 8;
    gb->registers[reg + 1] = value & 0xff;
}

// Return true if flag is set, false if flag is clear
bool gb_flag(byte_t flag, gb_system_t *gb)
{
    return (gb_register_read_byte(REG_F, gb) & (1 << flag)) ? true : false;
}

// Set flag
void gb_flag_set(byte_t flag, gb_system_t *gb)
{
    byte_t reg_value = gb_register_read_byte(REG_F, gb);

    gb_register_write_byte(REG_F, (reg_value | (1 << flag)), gb);
}

// Clear flag
void gb_flag_clear(byte_t flag, gb_system_t *gb)
{
    byte_t reg_value = gb_register_read_byte(REG_F, gb);

    gb_register_write_byte(REG_F, (reg_value ^ (1 << flag)), gb);
}