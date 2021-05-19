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
#include "mmu/mbc5.h"
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

static byte_t bootrom[256];

// Load bootrom from *filename
int mmu_load_bootrom(const char *filename)
{
    struct stat s;
    int fd, n, total;

    memset(bootrom, 0, sizeof(bootrom));
    if (stat(filename, &s) < 0) {
        logger(LOG_ERROR, "stat: %s: %s", filename, strerror(errno));
        return -1;
    }
    if (s.st_size != sizeof(bootrom)) {
        logger(LOG_ERROR, "%s: Invalid bootrom size: %li bytes", filename, s.st_size);
        return -1;
    }
    if ((fd = open(filename, OFLAG)) < 0) {
        logger(LOG_ERROR, "open: %s: %s", filename, strerror(errno));
        return -1;
    }

    total = 0;
    n = 0;
    while (total < s.st_size && (n = read(fd, &bootrom[total], s.st_size - total)) > 0)
        total += n;
    close(fd);

    if (n < 0) {
        logger(LOG_ERROR, "read: %s: %s", filename, strerror(errno));
        return -1;
    }
    if (total != s.st_size) {
        logger(LOG_ERROR, "%s: read %u bytes but expected %u bytes", filename, total, s.st_size);
        return -1;
    }
    return total;
}

// Read byte from bootrom
byte_t mmu_bootrom_readb(byte_t addr, __attribute__((unused)) gb_system_t *gb)
{
    logger(LOG_ALL, "mmu_bootrom: reading $%04X", addr);
    return bootrom[addr];
}

// Read byte from addr
byte_t mmu_readb(uint16_t addr, gb_system_t *gb)
{
    int16_t value;

    if (!gb->memory.bootrom_reg && addr <= 0xFF)
        return mmu_bootrom_readb(addr, gb);

    if (gb->memory.mbc_readb) {
        if ((value = (*gb->memory.mbc_readb)(addr, gb)) >= 0) {
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
        if ((value = (*gb->memory.mbc_readb)(addr, gb)) >= 0) {
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
    return (   gb->screen.lcdc.enable
            && (   gb->screen.lcd_stat.mode == LCDC_MODE_2
                || gb->screen.lcd_stat.mode == LCDC_MODE_3));
}

// Returns true if VRAM is inaccessible to the CPU
bool mmu_vram_blocked(gb_system_t *gb)
{
    return (gb->screen.lcdc.enable && gb->screen.lcd_stat.mode == LCDC_MODE_3);
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

    for (uint16_t i = 0; i < gb->memory.ram.banks_nb; ++i) {
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

    switch (gb->cartridge.mbc_type) {
        case 0x0F: // MBC3 + Timer + Battery
        case 0x10: // MBC3 + Timer + RAM + Battery
            write(fd, &((mbc3_regs_t *) gb->memory.mbc_regs)->rtc.rtc_s, 1);
            write(fd, &((mbc3_regs_t *) gb->memory.mbc_regs)->rtc.rtc_m, 1);
            write(fd, &((mbc3_regs_t *) gb->memory.mbc_regs)->rtc.rtc_h, 1);
            write(fd, &((mbc3_regs_t *) gb->memory.mbc_regs)->rtc.rtc_dl, 1);
            write(fd, &((mbc3_regs_t *) gb->memory.mbc_regs)->rtc.rtc_dh.b, 1);
            write(fd, &((mbc3_regs_t *) gb->memory.mbc_regs)->last_tick, sizeof(time_t));
            logger(LOG_DEBUG, "Saved MBC3 RTC registers");

        default: break;
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

    for (uint16_t i = 0; i < gb->memory.ram.banks_nb; ++i) {
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

    switch (gb->cartridge.mbc_type) {
        case 0x0F: // MBC3 + Timer + Battery
        case 0x10: // MBC3 + Timer + RAM + Battery
            if (   read(fd, &((mbc3_regs_t *) gb->memory.mbc_regs)->rtc.rtc_s, 1) != 1
                || read(fd, &((mbc3_regs_t *) gb->memory.mbc_regs)->rtc.rtc_m, 1) != 1
                || read(fd, &((mbc3_regs_t *) gb->memory.mbc_regs)->rtc.rtc_h, 1) != 1
                || read(fd, &((mbc3_regs_t *) gb->memory.mbc_regs)->rtc.rtc_dl, 1) != 1
                || read(fd, &((mbc3_regs_t *) gb->memory.mbc_regs)->rtc.rtc_dh.b, 1) != 1
                || read(fd, &((mbc3_regs_t *) gb->memory.mbc_regs)->last_tick, sizeof(time_t)) != sizeof(time_t))
            {
                logger(LOG_ERROR, "read(): %s: Failed to load MBC3 RTC registers: %s", gb->sav_file, strerror(errno));
                memset(gb->memory.mbc_regs, 0, sizeof(mbc3_regs_t));
            } else {
                logger(LOG_DEBUG, "Loaded MBC3 RTC registers");
                mbc3_rtc_tick_timestamp(gb);
            }
            break;

        default: break;
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
            gb->memory.mbc_readb = NULL;
            gb->memory.mbc_writeb = &mbc1_writeb;
            gb->memory.mbc_regs = xzalloc(sizeof(mbc1_regs_t));
            gb->memory.mbc_regs_size = sizeof(mbc1_regs_t);
            ((mbc1_regs_t *) gb->memory.mbc_regs)->large_ram = (gb->memory.ram.bank_size * gb->memory.ram.banks_nb) > RAM_BANK_SIZE;
            ((mbc1_regs_t *) gb->memory.mbc_regs)->large_rom = (gb->cartridge.rom_banks > 32);
            ((mbc1_regs_t *) gb->memory.mbc_regs)->rom_bank = 0x1;
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
            gb->memory.mbc_regs_size = sizeof(mbc3_regs_t);
            return true;

        case 0x1C: // MBC5 + Rumble
        case 0x1D: // MBC5 + Rumble + RAM
        case 0x1E: // MBC5 + Rumble + RAM + Battery
            logger(LOG_WARN, "Rumble is not supported\n");
            __attribute__((fallthrough));
        case 0x19: // MBC5
        case 0x1A: // MBC5 + RAM
        case 0x1B: // MBC5 + RAM + Battery
            gb->memory.mbc_writeb = &mbc5_writeb;
            gb->memory.mbc_regs = xzalloc(sizeof(mbc5_regs_t));
            gb->memory.mbc_regs_size = sizeof(mbc5_regs_t);
            gb->memory.mbc_battery = (mbc_type == 0x1E || mbc_type == 0x1B);
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