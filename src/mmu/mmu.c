/*
mmu.c
Memory Management Unit

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
#include "gameboy.h"
#include "mmu/mbc0.h"
#include <stdio.h>
#include <ctype.h>

// Read byte from addr
byte_t mmu_readb(uint16_t addr, gb_system_t *gb)
{
    if (gb->memory.readb_f) {
        logger(LOG_ALL, "mmu_readb: read address $%04X", addr);
        return (*gb->memory.readb_f)(addr, gb);
    } else {
        logger(LOG_ERROR, "mmu_readb failed: no readb handler");
        return 0;
    }
}

// Write byte at addr
bool mmu_writeb(uint16_t addr, byte_t value, gb_system_t *gb)
{
    if (gb->memory.writeb_f) {
        logger(LOG_ALL, "mmu_writeb: write $%02X at address $%04X", value, addr);
        return (*gb->memory.writeb_f)(addr, value, gb);
    } else {
        logger(LOG_ERROR, "mmu_writeb failed: no writeb handler");
        return false;
    }
}

// Read uint16 from addr
uint16_t mmu_read_u16(uint16_t addr, gb_system_t *gb)
{
    return (mmu_readb(addr, gb) | (mmu_readb(addr + 1, gb) << 8));
}

// Write uint16 at addr
bool mmu_write_u16(uint16_t addr, uint16_t value, gb_system_t *gb)
{
    bool tmp;

    tmp = mmu_writeb(addr, (value & 0xff), gb);
    return mmu_writeb(addr + 1, (value >> 8), gb) && tmp;
}

// Set MBC type and initialize MBC-related functions and variables
bool mmu_set_mbc(byte_t mbc_type, gb_system_t *gb)
{
    switch (mbc_type) {
        case 0x00: // MBC0 (no MBC, ROM only)
            gb->memory.readb_f = &mbc0_readb;
            gb->memory.writeb_f = &mbc0_writeb;
            return true;

        default: logger(LOG_ERROR, "Unsupported MBC type $%02X", mbc_type);
    }
    return false;
}

void mmu_dump(uint16_t addr, uint16_t n, gb_system_t *gb)
{
    const byte_t bytes_per_line = 16;
    byte_t buf[bytes_per_line];

    for (uint16_t i = 0; i < n; i += bytes_per_line, addr += bytes_per_line) {
        for (byte_t j = 0; j < bytes_per_line; ++j) {
            buf[j] = mmu_readb(addr + j, gb);
        }

        printf("0x%04X: ", addr);
        for (byte_t j = 0; j < bytes_per_line; ++j) {
            if (i + j < n) {
                printf("%02X ", buf[j]);
            } else {
                printf("   ");
            }

            if (j == (bytes_per_line / 2) - 1)
                printf(" ");
        }

        printf("   ");
        for (byte_t j = 0; j < bytes_per_line && i + j < n; ++j) {
            printf("%c", isprint((char) buf[j]) ? (char) buf[j] : '.');
        }
        printf("\n");
    }
}