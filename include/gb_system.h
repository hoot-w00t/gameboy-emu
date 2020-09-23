/*
gb_system.h
Function prototypes for gb_system.c

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

#ifndef _GB_SYSTEM_H
#define _GB_SYSTEM_H

byte_t *load_file(const char *filename, int *size);
int load_rom(byte_t *rom, int size, gb_system_t *gb);
int load_rom_from_file(const char *filename, gb_system_t *gb);
void gb_system_reset(gb_system_t *gb);
void gb_system_destroy(gb_system_t *gb);
gb_system_t *gb_system_create(void);
gb_system_t *gb_system_create_load_rom(const char *filename);

#endif