/*
memory.h
Header file for memory.c

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

#ifndef _GB_MEMORY_H
#define _GB_MEMORY_H

byte_t gb_read_byte(uint16_t address, gb_system_t *gb);
bool gb_write_byte(uint16_t address, byte_t value, bool bypass_ro, gb_system_t *gb);
uint16_t gb_read_address(uint16_t address, gb_system_t *gb);
int gb_load_rom(const char *filename, gb_system_t *gb);
bool gb_test_memory(uint16_t address, byte_t value, bool bypass_ro, gb_system_t *gb);

#endif