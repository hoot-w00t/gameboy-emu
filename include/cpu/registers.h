/*
registers.h
Function prototypes for registers.c

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

#ifndef _CPU_REGISTERS_H
#define _CPU_REGISTERS_H

byte_t reg_readb(byte_t reg, gb_system_t *gb);
void reg_writeb(byte_t reg, byte_t value, gb_system_t *gb);
uint16_t reg_read_u16(byte_t reg, gb_system_t *gb);
void reg_write_u16(byte_t reg, uint16_t value, gb_system_t *gb);
bool reg_flag(byte_t flag, gb_system_t *gb);
void reg_flag_set(byte_t flag, gb_system_t *gb);
void reg_flag_clear(byte_t flag, gb_system_t *gb);

#endif