/*
memory_banks.h
Header file for memory_banks.c

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

#ifndef _GB_MEMORY_BANKS_H
#define _GB_MEMORY_BANKS_H

void gb_free_membank(gb_membank_t *membank);
bool gb_allocate_membank(gb_membank_t *membank);
bool gb_switch_membank(byte_t index, gb_membank_t *membank);
bool gb_increment_membank(gb_membank_t *membank);
byte_t gb_read_byte_from_membank(uint16_t address, gb_membank_t *membank);
bool gb_write_byte_to_membank(uint16_t address, byte_t value, gb_membank_t *membank);

#endif