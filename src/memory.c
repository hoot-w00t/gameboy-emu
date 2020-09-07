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

#include "gb/defs.h"
#include "gb/memory_banks.h"
#include "gb/cartridge.h"
#include "gb/mbc.h"
#include "logger.h"
#include <stdio.h>
#include <stddef.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <ctype.h>

// Read byte from given address
byte_t gb_read_byte(uint16_t address, gb_system_t *gb)
{
    if (gb->memory.mbc_read && (*gb->memory.mbc_read)(address, gb)) {
        return true;
    }

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

    } else if (address >= IO_REGISTERS_LADDR && address <= IO_REGISTERS_UADDR) {
        return gb->memory.io_registers[address - IO_REGISTERS_LADDR];

    } else if (address >= HRAM_LADDR && address <= HRAM_UADDR) {
        return gb->memory.hram[address - HRAM_LADDR];

    } else if (address == INTERRUPT_ENABLE) {
        return gb->memory.int_enable;

    }

    logger(LOG_CRIT, "Unable to read byte at address: 0x%04X", address);
    return 0;
}

// Write byte at given address
// If bypass_ro is true, allow writing to read-only areas (also bypassing MBC)
// bypass_ro is used for loading ROMs
bool gb_write_byte(uint16_t address, byte_t value, bool bypass_ro, gb_system_t *gb)
{
    if (!bypass_ro && gb->memory.mbc_write && (*gb->memory.mbc_write)(address, value, gb)) {
        return true;
    }

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

    } else if (address >= IO_REGISTERS_LADDR && address <= IO_REGISTERS_UADDR) {
        gb->memory.io_registers[address - IO_REGISTERS_LADDR] = value;
        return true;

    } else if (address >= HRAM_LADDR && address <= HRAM_UADDR) {
        gb->memory.hram[address - HRAM_LADDR] = value;
        return true;

    } else if (address == INTERRUPT_ENABLE) {
        gb->memory.int_enable = value;
        return true;

    }

    logger(LOG_CRIT, "Unable to write byte at 0x%04X: invalid address", address);
    return false;
}

// Read address from memory
uint16_t gb_read_uint16(uint16_t address, gb_system_t *gb)
{
    return (uint16_t) (gb_read_byte(address, gb) | (gb_read_byte(address + 1, gb) << 8));
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

    uint16_t address = 0x0;
    uint16_t loaded_bytes = 0;
    byte_t buffer[1024];
    int n;

    gb->cartridge.filename = (char *) filename;

    while ((n = read(fd, buffer, sizeof(buffer))) > 0) {
        if (address == 0x0) {
            decode_cartridge_header(buffer, &gb->cartridge);

            byte_t header_checksum = compute_header_checksum(buffer);
            dump_cartridge_header(&gb->cartridge);
            logger(LOG_DEBUG, "Computed header checksum: 0x%02X", header_checksum);

            if (!valid_nintendo_logo(&gb->cartridge)) {
                logger(LOG_CRIT, "Invalid Nintendo Logo bitmap");
                close(fd);
                return -3;
            }

            if (header_checksum != gb->cartridge.header_checksum) {
                logger(LOG_CRIT, "Cartridge Header Checksum is invalid");
                close(fd);
                return -3;
            }

            gb->memory.rom_banks.bank_size = GB_ROM_BANK_SIZE;
            gb->memory.rom_banks.max_bank_nb = gb->cartridge.rom_banks;
            gb->memory.rom_banks.current_bank_nb = gb->cartridge.rom_banks;
            gb->memory.ram_banks.bank_size = gb->cartridge.ram_size;
            gb->memory.ram_banks.max_bank_nb = gb->cartridge.ram_banks;
            gb->memory.ram_banks.current_bank_nb = gb->cartridge.ram_banks;

            if (!gb_allocate_membank(&gb->memory.rom_banks) || !gb_allocate_membank(&gb->memory.ram_banks)) {
                close(fd);
                return -3;
            }

            gb->memory.mbc_read = get_mbc_readb_fptr(&gb->cartridge);
            gb->memory.mbc_write = get_mbc_writeb_fptr(&gb->cartridge);
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

    if (n < 0) {
        logger(LOG_ERROR, "Error while reading: %s: %s",
                          filename,
                          strerror(errno));
        return -2;
    }

    logger(LOG_DEBUG, "Computed global checksum: 0x%04X", compute_global_checksum(gb));

    gb_switch_membank(0, &gb->memory.rom_banks);
    close(fd);

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

void dump_memory(gb_system_t *gb, uint16_t address, const uint16_t bytes,
    const bool labels)
{
    const byte_t bytes_per_line = 16;
    byte_t b[bytes_per_line];

    for (uint16_t i = 0; i < bytes; i += bytes_per_line, address += bytes_per_line) {
        for (byte_t j = 0; j < bytes_per_line; ++j) {
            b[j] = gb_read_byte(address + j, gb);
        }

        if (labels) {
            if (address <= INTERRUPT_VECTOR_TABLE_UADDR) {
                printf("IVT       ");
            } else if (address >= CARTRIDGE_HEADER_LADDR && address <= CARTRIDGE_HEADER_UADDR) {
                printf("CART HDR  ");
            } else if (address >= ROM_BANK_0_LADDR && address <= ROM_BANK_0_UADDR) {
                printf("ROM #0    ");
            } else if (address >= ROM_BANK_N_LADDR && address <= ROM_BANK_N_UADDR) {
                printf("ROM #%3u  ", gb->memory.rom_banks.index + 1);
            } else if (address >= TILE_LADDR && address <= BG_MAP_2_UADDR) {
                printf("VRAM      ");
            } else if (address >= RAM_BANK_0_LADDR && address <= RAM_BANK_0_UADDR) {
                printf("RAM #0    ");
            } else if (address >= RAM_BANK_N_LADDR && address <= RAM_BANK_N_UADDR) {
                printf("RAM #%3u  ", gb->memory.ram_banks.index + 1);
            } else if (address >= RAM_ECHO_LADDR && address <= RAM_ECHO_UADDR) {
                printf("RAM Echo  ");
            } else if (address >= OAM_LADDR && address <= OAM_UADDR) {
                printf("OAM       ");
            } else if (address >= IO_REGISTERS_LADDR && address <= IO_REGISTERS_UADDR) {
                printf("IO REGS   ");
            } else if (address >= HRAM_LADDR && address <= HRAM_UADDR) {
                printf("HRAM      ");
            } else {
                printf("          ");
            }
        }

        printf("0x%04X: ", address);
        for (byte_t j = 0; j < bytes_per_line; ++j) {
            if (i + j < bytes) {
                printf("%02X ", b[j]);
            } else {
                printf("   ");
            }

            if (j == (bytes_per_line / 2) - 1)
                printf(" ");
        }

        printf("   ");
        for (byte_t j = 0; j < bytes_per_line && i + j < bytes; ++j) {
            printf("%c", isprint((char) b[j]) ? (char) b[j] : '.');
        }
        printf("\n");
    }
}

void dump_memory_range(gb_system_t *gb, uint16_t laddr, uint16_t uaddr,
    const bool labels)
{
    dump_memory(gb, laddr, uaddr - laddr, labels);
}