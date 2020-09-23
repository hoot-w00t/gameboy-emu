/*
gb_system.c
GameBoy system initialization

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

#include "xalloc.h"
#include "gameboy.h"
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>

#ifdef WIN32
#define OFLAG (O_RDONLY | O_BINARY)
#else
#define OFLAG (O_RDONLY)
#endif

// Load a ROM to gb_system_t
// Returns the amount of bytes read or -1 if it failed
int load_rom_from_file(const char *filename, gb_system_t *gb)
{
    return -1;
}

// Reset a gb_system_t to its startup state
void gb_system_reset(gb_system_t *gb)
{
    gb->pc = CARTRIDGE_HEADER_LADDR;
    gb->sp = HRAM_UADDR;
}

// Destroy gb_system_t and free all allocated memory
void gb_system_destroy(gb_system_t *gb)
{
    free(gb);
}

// Allocate and initialize an empty gb_system_t
gb_system_t *gb_system_create(void)
{
    gb_system_t *gb = xzalloc(sizeof(gb_system_t));

    gb_system_reset(gb);
    return gb;
}

// Create a gb_system_t and load ROM
// Returns NULL if the ROM failed to load
gb_system_t *gb_system_create_load_rom(const char *filename)
{
    gb_system_t *gb = gb_system_create();

    if (load_rom_from_file(filename, gb) < 0) {
        gb_system_destroy(gb);
        return NULL;
    }
    return gb;
}
