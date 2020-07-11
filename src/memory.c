/*
memory.c
GameBoy memory access functions

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
#include "memory_banks.h"
#include "cartridge.h"
#include "logger.h"
#include <stddef.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>

// Read byte from given address
byte_t gb_read_byte(uint16_t address, gb_system_t *gb)
{
    if (address <= ROM_BANK_0_UADDR) {
        return gb->memory.rom_bank_0[address];

    } else if (address >= ROM_BANK_N_LADDR && address <= ROM_BANK_N_UADDR) {
        return gb_read_byte_from_membank(
            address - ROM_BANK_N_LADDR,
            &gb->memory.rom_banks
        );

    } else if (address >= TILE_LADDR && address <= BG_MAP_2_UADDR) {
        return gb->memory.vram[address - TILE_LADDR];

    } else if (address >= RAM_BANK_0_LADDR && address <= RAM_BANK_0_UADDR) {
        return gb->memory.ram_bank_0[address - RAM_BANK_0_LADDR];

    } else if (address >= RAM_BANK_N_LADDR && address <= RAM_BANK_N_UADDR) {
        return gb_read_byte_from_membank(
            address - RAM_BANK_N_LADDR,
            &gb->memory.ram_banks
        );

    } else if (address >= OAM_LADDR && address <= OAM_UADDR) {
        return gb->memory.oam[address - OAM_LADDR];

    } else if (address >= HRAM_LADDR && address <= HRAM_UADDR) {
        return gb->memory.hram[address - HRAM_LADDR];

    }

    logger(LOG_CRIT, "Unable to read byte at address: 0x%04X", address);
    return 0;
}

// Write byte at given address
// If bypass_ro is true, allow writing to read-only areas
// bypass_ro is used for loading ROMs
bool gb_write_byte(uint16_t address, byte_t value, bool bypass_ro, gb_system_t *gb)
{
    if (address <= ROM_BANK_0_UADDR) {
        if (bypass_ro) {
            gb->memory.rom_bank_0[address] = value;
            return true;
        }

        logger(LOG_CRIT, "Unable to write byte at 0x%04X: address is read-only", address);
        return false;

    } else if (address >= ROM_BANK_N_LADDR && address <= ROM_BANK_N_UADDR) {
        if (bypass_ro) {
            return gb_write_byte_to_membank(
                address - ROM_BANK_N_LADDR,
                value,
                &gb->memory.rom_banks
            );
        }

        logger(LOG_CRIT, "Unable to write byte at 0x%04X: address is read-only", address);
        return false;

    } else if (address >= TILE_LADDR && address <= BG_MAP_2_UADDR) {
        return gb->memory.vram[address - TILE_LADDR];

    } else if (address >= RAM_BANK_0_LADDR && address <= RAM_BANK_0_UADDR) {
        return gb->memory.ram_bank_0[address - RAM_BANK_0_LADDR];

    } else if (address >= RAM_BANK_N_LADDR && address <= RAM_BANK_N_UADDR) {
        return gb_write_byte_to_membank(
            address - RAM_BANK_N_LADDR,
            value,
            &gb->memory.ram_banks
        );

    } else if (address >= OAM_LADDR && address <= OAM_UADDR) {
        gb->memory.oam[address - OAM_LADDR] = value;
        return true;

    } else if (address >= HRAM_LADDR && address <= HRAM_UADDR) {
        gb->memory.hram[address - HRAM_LADDR] = value;
        return true;

    }

    logger(LOG_CRIT, "Unable to write byte at 0x%04X: invalid address", address);
    return false;
}

// Load GameBoy ROM
int gb_load_rom(const char *filename, gb_system_t *gb)
{
    int fd;

    if ((fd = open(filename, O_RDONLY)) < 0) {
        logger(LOG_ERROR, "Unable to open: %s: %s",
                          filename,
                          strerror(errno));
        return -1;
    }

    uint16_t address = CARTRIDGE_HEADER_LADDR;
    uint16_t loaded_bytes = 0;
    byte_t buffer[1024];
    int n;

    while ((n = read(fd, buffer, sizeof(buffer))) > 0) {
        if (address == CARTRIDGE_HEADER_LADDR) {
            decode_cartridge_header(buffer, &gb->cartridge);

            logger(LOG_DEBUG, "Computed header checksum: 0x%02X", compute_header_checksum(buffer));
            logger(LOG_DEBUG, "Computed global checksum: 0x%04X", compute_global_checksum(buffer));
            dump_cartridge_header(&gb->cartridge);

            if (compute_header_checksum(buffer) != gb->cartridge.header_checksum) {
                logger(LOG_CRIT, "Cartridge Header Checksum is invalid");
                close(fd);
                return -3;
            }

            gb->memory.rom_banks.bank_size = GB_ROM_BANK_SIZE;
            gb->memory.rom_banks.maxsize = gb->cartridge.rom_banks;
            gb->memory.ram_banks.bank_size = gb->cartridge.ram_size;
            gb->memory.ram_banks.maxsize = gb->cartridge.ram_banks;

            if (!gb_allocate_membank(&gb->memory.rom_banks) || !gb_allocate_membank(&gb->memory.ram_banks)) {
                close(fd);
                return -3;
            }
        }

        for (int i = 0; i < n; ++i) {
            if (address >= ROM_BANK_N_UADDR) {
                address = ROM_BANK_N_LADDR;
                if (!gb_increment_membank(&gb->memory.rom_banks)) {
                    close(fd);
                    return -3;
                }
            }

            gb_write_byte(address + i, buffer[i], true, gb);
        }

        address += n;
        loaded_bytes += n;
    }

    gb_switch_membank(0, &gb->memory.rom_banks);
    close(fd);

    if (n < 0) {
        logger(LOG_ERROR, "Error while reading: %s: %s",
                          filename,
                          strerror(errno));
        return -2;
    }

    return loaded_bytes;
}

// Write value at given address and see whether it can be recovered
bool gb_test_memory(uint16_t address, byte_t value, bool bypass_ro, gb_system_t *gb)
{
    byte_t read_value;

    if (!gb_write_byte(address, value, bypass_ro, gb)) {
        return false;
    }
    if ((read_value = gb_read_byte(address, gb)) != value) {
        logger(LOG_CRIT, "gb_test_memory failed:  0x%04X has value 0x%02X", address, read_value);
        return false;
    }

    return true;
}