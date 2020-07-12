/*
registers.h
Header file for registers.c

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

#ifndef _GB_REGISTERS_H
#define _GB_REGISTERS_H

byte_t gb_register_read_byte(byte_t reg, gb_system_t *gb);
void gb_register_write_byte(byte_t reg, byte_t value, gb_system_t *gb);
uint16_t gb_register_read_u16(byte_t reg, gb_system_t *gb);
void gb_register_write_u16(byte_t reg, uint16_t value, gb_system_t *gb);

#endif