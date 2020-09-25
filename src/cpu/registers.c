/*
registers.c
CPU Registers and flags

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

// Read byte from a CPU register
byte_t reg_readb(byte_t reg, gb_system_t *gb)
{
    return gb->registers[reg];
}

// Write byte to a CPU register
void reg_writeb(byte_t reg, byte_t value, gb_system_t *gb)
{
    gb->registers[reg] = value;
}

// Read 16-bit value from a 16 bit CPU register
uint16_t reg_read_u16(byte_t reg, gb_system_t *gb)
{
    return (gb->registers[reg] << 8) | gb->registers[reg + 1];
}

// Write 16-bit value to a 16 bit CPU register
void reg_write_u16(byte_t reg, uint16_t value, gb_system_t *gb)
{
    gb->registers[reg] = (value >> 8) & 0xff;
    gb->registers[reg + 1] = value & 0xff;
}

// Return true if flag is set, false if flag is clear
bool reg_flag(byte_t flag, gb_system_t *gb)
{
    return (reg_readb(REG_F, gb) & (1 << flag)) != 0;
}

// Set flag
void reg_flag_set(byte_t flag, gb_system_t *gb)
{
    byte_t reg_value = reg_readb(REG_F, gb);

    reg_writeb(REG_F, (reg_value | (1 << flag)), gb);
}

// Clear flag
void reg_flag_clear(byte_t flag, gb_system_t *gb)
{
    byte_t reg_value;

    if (reg_flag(flag, gb)) {
        reg_value = reg_readb(REG_F, gb);
        reg_writeb(REG_F, (reg_value ^ (1 << flag)), gb);
    }
}

// Set or clear N flag (Subtract flag)
void reg_flag_n(const bool subtract, gb_system_t *gb)
{
    if (subtract) {
        reg_flag_set(FLAG_N, gb);
    } else {
        reg_flag_clear(FLAG_N, gb);
    }
}