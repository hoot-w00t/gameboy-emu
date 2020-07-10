/*
emulator.c
Emulator's main loop

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

#include "gb_defs.h"
#include "logger.h"
#include "memory.h"
#include "memory_banks.h"
#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include <getopt.h>

// Free gb_system_t
// If include_ptr is true, also free *gb
void free_gb_system(gb_system_t *gb, bool include_ptr)
{
    gb_free_membank(&gb->memory.rom_banks);
    gb_free_membank(&gb->memory.ram_banks);

    if (include_ptr)
        free(gb);
}

// Allocate and initialize gb_system_t
gb_system_t *create_gb_system(byte_t rom_banks)
{
    gb_system_t *gb = malloc(sizeof(gb_system_t));

    if (!gb)
        return NULL;

    memset(gb, 0, sizeof(gb_system_t));
    gb->memory.rom_banks.bank_size = GB_ROM_BANK_SIZE;
    gb->memory.rom_banks.maxsize = rom_banks;
    gb->memory.ram_banks.bank_size = GB_RAM_BANK_SIZE;
    gb->memory.ram_banks.maxsize = 7;

    if (!gb_allocate_membank(&gb->memory.rom_banks) || !gb_allocate_membank(&gb->memory.ram_banks)) {
        free(gb);
        return NULL;
    }

    gb->pc = CARTRIDGE_HEADER_LADDR;
    gb->sp = HRAM_UADDR;

    return gb;
}

void parse_args(int ac, char **av, char **filename, int *rom_banks)
{
    const char shortopts[] = "l:b:";
    int opt;

    while ((opt = getopt(ac, av, shortopts)) >= 0) {
        switch (opt) {
            case 'l':
                if (set_logger_level(optarg) < 0) {
                    fprintf(stderr, "Invalid log level\n");
                    exit(EXIT_FAILURE);
                }
                break;

            case 'b':
                *rom_banks = atoi(optarg);
                break;

            default: exit(EXIT_FAILURE);
        }
    }

    if (optind >= ac) {
        fprintf(stderr, "Missing positionnal argument: filename\n");
        exit(EXIT_FAILURE);
    }

    *filename = av[optind];
}

int main(int ac, char **av)
{
    gb_system_t *gb;
    char *filename;
    int rom_banks = 1;

    parse_args(ac, av, &filename, &rom_banks);
    if ((gb = create_gb_system((byte_t) rom_banks)) == NULL) {
        fprintf(stderr, "Memory allocation failed");
        return EXIT_FAILURE;
    }

    if (gb_load_rom(filename, gb) < 0)
        return EXIT_FAILURE;

    

    free(gb);
    return 0;
}