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
#include "cpu/registers.h"
#include "mmu/mmu.h"
#include "mmu/rombanks.h"
#include "mmu/rambanks.h"
#include "ppu/lcd_regs.h"
#include "apu/sound_regs.h"
#include "timer.h"
#include "joypad.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <libgen.h>

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
    uint32_t rom_bytes;

    if (!cartridge_decode_hdr(rom, &gb->cartridge))
        return -1;

    if (!cartridge_check_logo(&gb->cartridge)) {
        logger(LOG_ERROR, "load_rom: Nintendo Bitmap Logo is invalid");
        return -2;
    }

    if ((hdr_checksum = compute_header_checksum(rom)) != gb->cartridge.header_checksum) {
        logger(LOG_ERROR, "load_rom: Header checksum is invalid (expected $%02X, got $%02X)",
            gb->cartridge.header_checksum,
            hdr_checksum);
        return -3;
    }

    if ((rom_bytes = (gb->cartridge.rom_banks * ROM_BANK_SIZE)) != size) {
        logger(LOG_ERROR, "load_rom: Expected ROM of %u bytes but %i bytes are loaded",
            rom_bytes,
            size);
        return -4;
    }
    if (!mmu_set_mbc(gb->cartridge.mbc_type, gb))
        return -5;

    rombank_alloc(gb->cartridge.rom_banks, &gb->memory.rom);
    rambank_alloc(gb->cartridge.ram_banks, gb->cartridge.ram_size, &gb->memory.ram);
    for (uint16_t i = 0; i < gb->memory.rom.banks_nb; ++i)
        memcpy(gb->memory.rom.banks[i], (rom + (ROM_BANK_SIZE * i)), ROM_BANK_SIZE);

    return 0;
}

// Returns a pointer to the start of the extension (excluding the dot)
// Returns NULL if there is no extension
char *filename_ext(char *filename)
{
    size_t i = strlen(filename);

    for (; i > 0; --i) {
        if (filename[i - 1] == '.')
            return &filename[i];
    }
    return NULL;
}

// Load a ROM to gb_system_t
// Returns the amount of bytes read or -1 if it failed
int load_rom_from_file(const char *filename, gb_system_t *gb)
{
    byte_t *rom;
    int size, ret;

    if (!(rom = load_file(filename, &size)))
        return -1;

    if ((size % ROM_BANK_SIZE) != 0) {
        logger(LOG_ERROR, "%s: Not a valid ROM file", filename);
        free(rom);
        return -1;
    }

    ret = load_rom(rom, size, gb);
    free(rom);
    if (ret < 0) {
        return ret;
    } else {
        char *tmp_filename = xstrdup(filename);
        char *tmp_basename = basename(tmp_filename);
        char *tmp_ext = filename_ext(tmp_basename);
        size_t ext_len = tmp_ext ? strlen(tmp_ext) : 0;
        size_t filename_wo_ext_len = strlen(filename) - ext_len;

        gb->sav_file = xzalloc(sizeof(char) * (filename_wo_ext_len + 5));
        strncpy(gb->sav_file, filename, filename_wo_ext_len);
        if (gb->sav_file[filename_wo_ext_len - 1] != '.') {
            strcat(gb->sav_file, ".sav");
        } else {
            strcat(gb->sav_file, "sav");
        }
        gb->rom_file = xstrdup(filename);

        free(tmp_filename);
        return size;
    }
    return ret < 0 ? ret : size;
}

// Reset a gb_system_t to its startup state
void gb_system_reset(bool enable_bootrom, gb_system_t *gb)
{
    if (enable_bootrom) {
        gb->memory.bootrom_reg = 0;
        gb->pc = 0x0;
    } else {
        gb->memory.bootrom_reg = 1;
        gb->pc = CARTRIDGE_HEADER_LADDR;

        // Initialize registers
        reg_write_u16(REG_AF, 0x01B0, gb);
        reg_write_u16(REG_BC, 0x0013, gb);
        reg_write_u16(REG_DE, 0x00D8, gb);
        reg_write_u16(REG_HL, 0x014D, gb);
        gb->sp = HRAM_UADDR;

        // Initialize IO registers
        // Joypad
        memset(&gb->joypad, 0, sizeof(gb->joypad));

        // Timer
        timer_reg_writeb(TIM_TIMA, 0, gb);
        timer_reg_writeb(TIM_TMA, 0, gb);
        timer_reg_writeb(TIM_TAC, 0, gb);

        // Sound
        (*((byte_t *) &gb->apu.regs.nr10)) = 0x80;
        (*((byte_t *) &gb->apu.regs.nr11)) = 0xBF;
        (*((byte_t *) &gb->apu.regs.nr12)) = 0xF3;
        (*((byte_t *) &gb->apu.regs.nr14)) = 0xBF;
        (*((byte_t *) &gb->apu.regs.nr21)) = 0x3F;
        (*((byte_t *) &gb->apu.regs.nr22)) = 0x00;
        (*((byte_t *) &gb->apu.regs.nr24)) = 0xBF;
        (*((byte_t *) &gb->apu.regs.nr30)) = 0x7F;
        (*((byte_t *) &gb->apu.regs.nr31)) = 0xFF;
        (*((byte_t *) &gb->apu.regs.nr32)) = 0x9F;
        (*((byte_t *) &gb->apu.regs.nr34)) = 0xBF;
        (*((byte_t *) &gb->apu.regs.nr41)) = 0xFF;
        (*((byte_t *) &gb->apu.regs.nr42)) = 0x00;
        (*((byte_t *) &gb->apu.regs.nr43)) = 0x00;
        (*((byte_t *) &gb->apu.regs.nr44)) = 0xBF;
        (*((byte_t *) &gb->apu.regs.nr50)) = 0x77;
        (*((byte_t *) &gb->apu.regs.nr51)) = 0xF3;
        (*((byte_t *) &gb->apu.regs.nr52)) = 0xF1;

        // LCD
        memset(&gb->screen, 0, sizeof(struct lcd_screen));
        lcd_reg_writeb(LCDC,      0x91, gb);
        lcd_reg_writeb(LCDC_SCY,  0x00, gb);
        lcd_reg_writeb(LCDC_SCX,  0x00, gb);
        lcd_reg_writeb(LCDC_LYC,  0x00, gb);
        lcd_reg_writeb(LCDC_BGP,  0xFC, gb);
        lcd_reg_writeb(LCDC_OBP0, 0xFF, gb);
        lcd_reg_writeb(LCDC_OBP1, 0xFF, gb);
        lcd_reg_writeb(LCDC_WY,   0x00, gb);
        lcd_reg_writeb(LCDC_WX,   0x00, gb);

        // Interrupts
        gb->interrupts.ie_reg = 0x00;
    }
}

// Destroy gb_system_t and free all allocated memory
void gb_system_destroy(gb_system_t *gb)
{
    rombank_free(&gb->memory.rom);
    rambank_free(&gb->memory.ram);
    free(gb->memory.mbc_regs);
    free(gb->rom_file);
    free(gb->sav_file);
    free(gb);
}

// Allocate and initialize an empty gb_system_t
gb_system_t *gb_system_create(bool enable_bootrom)
{
    gb_system_t *gb = xzalloc(sizeof(gb_system_t));

    gb_system_reset(enable_bootrom, gb);
    return gb;
}

// Create a gb_system_t and load ROM
// Returns NULL if the ROM failed to load
gb_system_t *gb_system_create_load_rom(const char *filename, bool enable_bootrom)
{
    gb_system_t *gb = gb_system_create(enable_bootrom);

    if (load_rom_from_file(filename, gb) < 0) {
        gb_system_destroy(gb);
        return NULL;
    }
    return gb;
}
