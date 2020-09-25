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

#include "logger.h"
#include "xalloc.h"
#include "gameboy.h"
#include "cartridge.h"
#include "mmu/mmu.h"
#include "mmu/banks.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#ifdef WIN32
#define OFLAG (O_RDONLY | O_BINARY)
#else
#define OFLAG (O_RDONLY)
#endif

byte_t *load_file(const char *filename, int *size)
{
    struct stat inf;
    byte_t *data;
    int fd, n, total;

    if (stat(filename, &inf) < 0) {
        logger(LOG_ERROR,
               "stat: %s: %s",
               filename,
               strerror(errno));
        return NULL;
    }
    *size = inf.st_size;

    if ((fd = open(filename, OFLAG)) < 0) {
        logger(LOG_ERROR,
               "open: %s: %s",
               filename,
               strerror(errno));
        return NULL;
    }

    data = xzalloc(sizeof(byte_t) * inf.st_size);
    total = 0;
    while (total < inf.st_size && (n = read(fd, &data[total], inf.st_size - total)) > 0) {
        total += n;
    }
    close(fd);

    if (n < 0) {
        logger(LOG_ERROR,
               "read: %s: %s",
               filename,
               strerror(errno));
        free(data);
        return NULL;
    }
    if (total != inf.st_size) {
        logger(LOG_ERROR,
               "%s: read %uB but expected %uB",
               filename,
               total,
               inf.st_size);
        free(data);
        return NULL;
    }

    return data;
}

// Load ROM from byte array
// Returns < 0 on failure
int load_rom(byte_t *rom, int size, gb_system_t *gb)
{
    byte_t hdr_checksum;
    uint16_t addr, bank_index;

    if (!cartridge_decode_hdr(rom, &gb->cartridge))
        return -1;

    if (!cartridge_check_logo(&gb->cartridge)) {
        logger(LOG_ERROR, "load_rom: Nintendo Bitmap Logo is invalid");
        return -2;
    }
    if ((hdr_checksum = compute_header_checksum(rom)) != gb->cartridge.header_checksum) {
        logger(LOG_ERROR,
               "load_rom: Header checksum is invalid (expected $%02X, got $%02X)",
               gb->cartridge.header_checksum,
               hdr_checksum);
        return -3;
    }

    if (!mmu_set_mbc(gb->cartridge.mbc_type, gb))
        return -4;

    membank_init(gb->cartridge.rom_banks, ROM_BANK_SIZE, ROM_BANK_SIZE, &gb->memory.rom);
    membank_init(gb->cartridge.ram_banks + 1, RAM_BANK_SIZE, gb->cartridge.ram_size, &gb->memory.ram);

    addr = 0x0; bank_index = 0;
    for (int i = 0; i < size; ++i, ++addr) {
        if (addr >= ROM_BANK_SIZE) {
            addr = 0x0;
            ++bank_index;
        }

        if (!membank_writeb_bank(addr, rom[i], bank_index, &gb->memory.rom))
            return -5;
    }

    return 0;
}

// Load a ROM to gb_system_t
// Returns the amount of bytes read or -1 if it failed
int load_rom_from_file(const char *filename, gb_system_t *gb)
{
    byte_t *rom;
    int size, ret;

    if (!(rom = load_file(filename, &size)))
        return -1;

    if (size < ROM_BANK_SIZE) {
        logger(LOG_ERROR,
               "%s: not a valid ROM file",
               filename);
        free(rom);
        return -1;
    }

    ret = load_rom(rom, size, gb);
    free(rom);
    return ret < 0 ? ret : size;
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
    membank_free(&gb->memory.rom);
    membank_free(&gb->memory.ram);
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
