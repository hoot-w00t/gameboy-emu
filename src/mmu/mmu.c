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
#include "xalloc.h"
#include "gameboy.h"
#include "mmu/mmu_internal.h"
#include "mmu/mbc1.h"
#include "mmu/mbc3.h"
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <ctype.h>

#ifdef WIN32
#define OFLAG (O_BINARY)
#else
#define OFLAG (0)
#endif

// dmg_boot.bin from https://gbdev.gg8.se/files/roms/bootroms/
// Converted to byte_t array using xxd -i
static const byte_t bootrom_dmg[256] = {
    0x31, 0xfe, 0xff, 0xaf, 0x21, 0xff, 0x9f, 0x32, 0xcb, 0x7c, 0x20, 0xfb,
    0x21, 0x26, 0xff, 0x0e, 0x11, 0x3e, 0x80, 0x32, 0xe2, 0x0c, 0x3e, 0xf3,
    0xe2, 0x32, 0x3e, 0x77, 0x77, 0x3e, 0xfc, 0xe0, 0x47, 0x11, 0x04, 0x01,
    0x21, 0x10, 0x80, 0x1a, 0xcd, 0x95, 0x00, 0xcd, 0x96, 0x00, 0x13, 0x7b,
    0xfe, 0x34, 0x20, 0xf3, 0x11, 0xd8, 0x00, 0x06, 0x08, 0x1a, 0x13, 0x22,
    0x23, 0x05, 0x20, 0xf9, 0x3e, 0x19, 0xea, 0x10, 0x99, 0x21, 0x2f, 0x99,
    0x0e, 0x0c, 0x3d, 0x28, 0x08, 0x32, 0x0d, 0x20, 0xf9, 0x2e, 0x0f, 0x18,
    0xf3, 0x67, 0x3e, 0x64, 0x57, 0xe0, 0x42, 0x3e, 0x91, 0xe0, 0x40, 0x04,
    0x1e, 0x02, 0x0e, 0x0c, 0xf0, 0x44, 0xfe, 0x90, 0x20, 0xfa, 0x0d, 0x20,
    0xf7, 0x1d, 0x20, 0xf2, 0x0e, 0x13, 0x24, 0x7c, 0x1e, 0x83, 0xfe, 0x62,
    0x28, 0x06, 0x1e, 0xc1, 0xfe, 0x64, 0x20, 0x06, 0x7b, 0xe2, 0x0c, 0x3e,
    0x87, 0xe2, 0xf0, 0x42, 0x90, 0xe0, 0x42, 0x15, 0x20, 0xd2, 0x05, 0x20,
    0x4f, 0x16, 0x20, 0x18, 0xcb, 0x4f, 0x06, 0x04, 0xc5, 0xcb, 0x11, 0x17,
    0xc1, 0xcb, 0x11, 0x17, 0x05, 0x20, 0xf5, 0x22, 0x23, 0x22, 0x23, 0xc9,
    0xce, 0xed, 0x66, 0x66, 0xcc, 0x0d, 0x00, 0x0b, 0x03, 0x73, 0x00, 0x83,
    0x00, 0x0c, 0x00, 0x0d, 0x00, 0x08, 0x11, 0x1f, 0x88, 0x89, 0x00, 0x0e,
    0xdc, 0xcc, 0x6e, 0xe6, 0xdd, 0xdd, 0xd9, 0x99, 0xbb, 0xbb, 0x67, 0x63,
    0x6e, 0x0e, 0xec, 0xcc, 0xdd, 0xdc, 0x99, 0x9f, 0xbb, 0xb9, 0x33, 0x3e,
    0x3c, 0x42, 0xb9, 0xa5, 0xb9, 0xa5, 0x42, 0x3c, 0x21, 0x04, 0x01, 0x11,
    0xa8, 0x00, 0x1a, 0x13, 0xbe, 0x20, 0xfe, 0x23, 0x7d, 0xfe, 0x34, 0x20,
    0xf5, 0x06, 0x19, 0x78, 0x86, 0x23, 0x05, 0x20, 0xfb, 0x86, 0x20, 0xfe,
    0x3e, 0x01, 0xe0, 0x50
};

// TODO: Allow the user to load a bootrom
// Read byte from bootrom
byte_t mmu_bootrom_readb(byte_t addr, __attribute__((unused)) gb_system_t *gb)
{
    logger(LOG_ALL, "mmu_bootrom: reading $%04X", addr);
    return bootrom_dmg[addr];
}

// Read byte from addr
byte_t mmu_readb(uint16_t addr, gb_system_t *gb)
{
    int16_t value;

    if (!gb->memory.bootrom_reg && addr <= 0xFF)
        return mmu_bootrom_readb(addr, gb);

    if (gb->memory.mbc_readb) {
        if ((value = (*gb->memory.mbc_readb)(addr, gb)) > 0) {
            logger(LOG_ALL, "mmu_readb: read $%02X from address $%04X", (byte_t) value, addr);
            return (byte_t) value;
        }
    }
    value = mmu_internal_readb(addr, gb);
    logger(LOG_ALL, "mmu_readb: read $%02X from address $%04X", value, addr);
    return value;
}

// Read byte from addr without logging
byte_t mmu_readb_nolog(uint16_t addr, gb_system_t *gb)
{
    int16_t value;

    if (!gb->memory.bootrom_reg && addr <= 0xFF)
        return mmu_bootrom_readb(addr, gb);

    if (gb->memory.mbc_readb) {
        if ((value = (*gb->memory.mbc_readb)(addr, gb)) > 0) {
            return (byte_t) value;
        }
    }
    return mmu_internal_readb(addr, gb);
}

// Write byte at addr
bool mmu_writeb(uint16_t addr, byte_t value, gb_system_t *gb)
{
    logger(LOG_ALL, "mmu_writeb: write $%02X at address $%04X", value, addr);
    if (gb->memory.mbc_writeb) {
        if ((*gb->memory.mbc_writeb)(addr, value, gb))
            return true;
    }
    return mmu_internal_writeb(addr, value, gb);
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

// Returns true if OAM is inaccessible to the CPU
bool mmu_oam_blocked(gb_system_t *gb)
{
    return false; // TODO: Revert when timings are accurate
    return (gb->screen.enable && (gb->screen.mode == LCDC_MODE_2 || gb->screen.mode == LCDC_MODE_3));
}

bool mmu_vram_blocked(gb_system_t *gb)
{
    return false; // TODO: Revert when timings are accurate
    return (gb->screen.enable && gb->screen.mode == LCDC_MODE_3);
}

// Save RAM banks to a battery file
bool mmu_battery_save(gb_system_t *gb)
{
    int fd;
    size_t offset;
    ssize_t n;

    if ((fd = open(gb->sav_file, OFLAG | O_WRONLY | O_CREAT,
                                 S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH)) < 0) {
        fprintf(stderr, "open(): %s: %s\n", gb->sav_file, strerror(errno));
        return false;
    }

    for (uint16_t i = 1; i < gb->memory.ram.max_bank_nb; ++i) {
        offset = 0;

        while (offset < gb->memory.ram.bank_size) {
            if ((n = write(fd, gb->memory.ram.banks[i] + offset,
                               gb->memory.ram.bank_size - offset)) <= 0) {
                fprintf(stderr, "write(): %s: %s\n", gb->sav_file, strerror(errno));
                close(fd);
                return false;
            }

            logger(LOG_DEBUG, "Saved chunk of %li bytes (RAM bank %u)", n, i);
            offset += n;
        }
    }
    logger(LOG_INFO, "Saved battery to %s", gb->sav_file);

    close(fd);
    return true;
}

// Load RAM banks from a battery file
bool mmu_battery_load(gb_system_t *gb)
{
    int fd;
    size_t offset;
    ssize_t n;

    if ((fd = open(gb->sav_file, OFLAG | O_RDONLY)) < 0) {
        fprintf(stderr, "open(): %s: %s\n", gb->sav_file, strerror(errno));
        return false;
    }

    for (uint16_t i = 1; i < gb->memory.ram.max_bank_nb; ++i) {
        offset = 0;

        while (offset < gb->memory.ram.bank_size) {
            if ((n = read(fd, gb->memory.ram.banks[i] + offset,
                               gb->memory.ram.bank_size - offset)) <= 0) {
                fprintf(stderr, "read(): %s: %s\n", gb->sav_file, strerror(errno));
                close(fd);
                return false;
            }

            logger(LOG_DEBUG, "Loaded chunk of %li bytes (RAM bank %u)", n, i);
            offset += n;
        }
    }
    logger(LOG_INFO, "Loaded battery from %s", gb->sav_file);

    close(fd);
    return true;
}

// Set MBC type and initialize MBC-related functions and variables
bool mmu_set_mbc(byte_t mbc_type, gb_system_t *gb)
{
    switch (mbc_type) {
        case 0x00: // MBC0 (no MBC, ROM only)
            gb->memory.mbc_readb = NULL;
            gb->memory.mbc_writeb = NULL;
            return true;

        case 0x03: // MBC1 + RAM + Battery
            gb->memory.mbc_battery = true;
            __attribute__((fallthrough));
        case 0x01: // MBC1
        case 0x02: // MBC1 + RAM
            gb->memory.mbc_readb = &mbc1_readb;
            gb->memory.mbc_writeb = &mbc1_writeb;
            gb->memory.mbc_regs = xzalloc(sizeof(mbc1_regs_t));
            ((mbc1_regs_t *) gb->memory.mbc_regs)->large_ram = (gb->memory.ram.bank_size * gb->memory.ram.max_bank_nb) > 8192;
            ((mbc1_regs_t *) gb->memory.mbc_regs)->large_rom = (gb->cartridge.rom_banks > 32);
            if (gb->cartridge.rom_banks <= 0x1) {
                ((mbc1_regs_t *) gb->memory.mbc_regs)->rom_mask = 0x1;
            } else if (gb->cartridge.rom_banks <= 0x3) {
                ((mbc1_regs_t *) gb->memory.mbc_regs)->rom_mask = 0x3;
            } else if (gb->cartridge.rom_banks <= 0x7) {
                ((mbc1_regs_t *) gb->memory.mbc_regs)->rom_mask = 0x7;
            } else if (gb->cartridge.rom_banks <= 0xF) {
                ((mbc1_regs_t *) gb->memory.mbc_regs)->rom_mask = 0xF;
            } else {
                ((mbc1_regs_t *) gb->memory.mbc_regs)->rom_mask = 0x1F;
            }
            return true;

        case 0x0F: // MBC3 + Timer + Battery
        case 0x10: // MBC3 + Timer + RAM + Battery
        case 0x13: // MBC3 + RAM + Battery
            gb->memory.mbc_battery = true;
            __attribute__((fallthrough));
        case 0x12: // MBC3 + RAM
        case 0x11: // MBC3
            gb->memory.mbc_readb = &mbc3_readb;
            gb->memory.mbc_writeb = &mbc3_writeb;
            gb->memory.mbc_clock = &mbc3_clock;
            gb->memory.mbc_regs = xzalloc(sizeof(mbc3_regs_t));
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