/*
mmu_internal.c
Internal MMU

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
#include "mmu/mmu.h"
#include "mmu/rombanks.h"
#include "mmu/rambanks.h"
#include "timer.h"
#include "ppu/lcd_regs.h"
#include "apu/sound_regs.h"
#include "joypad.h"
#include "serial.h"

byte_t mmu_internal_readb(uint16_t addr, gb_system_t *gb)
{
    switch (addr >> 8) {
        case 0x00 ... 0x3F: // ROM 0
            return gb->memory.rom.banks[gb->memory.rom.bank_0][addr];

        case 0x40 ... 0x7F: // ROM Bank
            return gb->memory.rom.banks[gb->memory.rom.bank_n][addr - ROM_BANK_N_LADDR];

        case 0x80 ... 0x9F: // VRAM
            if (mmu_vram_blocked(gb)) {
                logger(LOG_ERROR, "mmu_readb failed: address $%04X: VRAM is not accessible", addr);
                return MMU_UNMAPPED_ADDR_VALUE;
            }
            return gb->memory.vram[addr - VRAM_LADDR];

        case 0xA0 ... 0xBF: // RAM Bank
            if (!rambank_exists(&gb->memory.ram)) {
                logger(LOG_ERROR, "mmu_readb failed: address $%04X: No RAM banks are available", addr);
                return MMU_UNMAPPED_ADDR_VALUE;
            }
            if (!gb->memory.ram.can_read) {
                logger(LOG_ERROR, "mmu_readb failed: address $%04X: Reading is disabled for RAM banks", addr);
                return MMU_UNMAPPED_ADDR_VALUE;
            }
            if ((addr - RAM_BANK_N_LADDR) >= gb->memory.ram.bank_size) {
                logger(LOG_WARN, "mmu_readb failed: address $%04X: Out of bounds, bank only has %u bytes",
                    addr, gb->memory.ram.bank_size);
                return MMU_UNMAPPED_ADDR_VALUE;
            }
            return gb->memory.ram.banks[gb->memory.ram.bank][addr - RAM_BANK_N_LADDR];

        case 0xC0 ... 0xDF: // Work RAM
            return gb->memory.wram[addr - RAM_BANK_0_LADDR];

        case 0xE0 ... 0xFD: // Echo RAM
            return gb->memory.wram[addr - RAM_ECHO_LADDR];

        case 0xFE: // OAM
                if (addr <= OAM_UADDR) {
                    if (mmu_oam_blocked(gb)) {
                        logger(LOG_ERROR, "mmu_readb failed: address $%04X: OAM is not accessible", addr);
                        return MMU_UNMAPPED_ADDR_VALUE;
                    }
                    return gb->memory.oam[addr & 0xFF];
                } else {
                    return mmu_oam_blocked(gb) ? 0xFF : 0x00;
                }

        case 0xFF:
            switch (addr & 0xFF) {
                case 0x00:
                    return joypad_reg_readb(gb);

                case 0x01:
                case 0x02:
                    return serial_reg_readb(addr, gb);

                case 0x04 ... 0x07:
                    return timer_reg_readb(addr, gb);

                case 0x0F:
                    return gb->interrupts.if_reg | 0xE0;

                case 0x10 ... 0x14:
                case 0x16 ... 0x19:
                case 0x1A ... 0x1E:
                case 0x20 ... 0x23:
                case 0x24 ... 0x26:
                case 0x30 ... 0x3F:
                    return sound_reg_readb(addr, gb);

                case 0x40 ... 0x4B:
                    return lcd_reg_readb(addr, gb);

                case 0x50:
                    return gb->memory.bootrom_reg | 0xFE;

                case 0x80 ... 0xFE: // HRAM
                    return gb->memory.hram[addr - HRAM_LADDR];

                case 0xFF:
                    return gb->interrupts.ie_reg;
            }
    }
    logger(LOG_WARN, "mmu_readb failed: address $%04X", addr);
    return MMU_UNMAPPED_ADDR_VALUE;
}

bool mmu_internal_writeb(uint16_t addr, byte_t value, gb_system_t *gb)
{
    switch (addr >> 8) {
        case 0x00 ... 0x7F: // ROM Banks
            logger(LOG_ERROR, "mmu_writeb failed: address $%04X is read-only", addr);
            return false;

        case 0x80 ... 0x9F: // VRAM
            if (mmu_vram_blocked(gb)) {
                logger(LOG_ERROR, "mmu_writeb failed: address $%04X: VRAM is not accessible", addr);
                return false;
            }
            gb->memory.vram[addr - VRAM_LADDR] = value;
            return true;

        case 0xA0 ... 0xBF: // RAM Bank
            if (!rambank_exists(&gb->memory.ram)) {
                logger(LOG_ERROR, "mmu_writeb failed: address $%04X: No RAM banks are available", addr);
                return false;
            }
            if (!gb->memory.ram.can_write) {
                logger(LOG_ERROR, "mmu_writeb failed: address $%04X: Writing is disabled for RAM banks", addr);
                return false;
            }
            if ((addr - RAM_BANK_N_LADDR) >= gb->memory.ram.bank_size) {
                logger(LOG_WARN, "mmu_writeb failed: address $%04X: Out of bounds, bank only has %u bytes",
                    addr, gb->memory.ram.bank_size);
                return false;
            }
            gb->memory.ram.banks[gb->memory.ram.bank][addr - RAM_BANK_N_LADDR] = value;
            return true;

        case 0xC0 ... 0xDF: // Work RAM
            gb->memory.wram[addr - RAM_BANK_0_LADDR] = value;
            return true;

        case 0xE0 ... 0xFD: // Echo RAM
            gb->memory.wram[addr - RAM_ECHO_LADDR] = value;
                return true;

        case 0xFE: // OAM
                if (addr <= OAM_UADDR) {
                    if (mmu_oam_blocked(gb)) {
                        logger(LOG_ERROR, "mmu_writeb failed: address $%04X: OAM is not accessible", addr);
                        return false;
                    }
                    gb->memory.oam[addr & 0xFF] = value;

                }
                return true;

        case 0xFF:
            switch (addr & 0xFF) {
                case 0x00:
                    return joypad_reg_writeb(value, gb);

                case 0x01:
                case 0x02:
                    return serial_reg_writeb(addr, value, gb);

                case 0x04 ... 0x07:
                    return timer_reg_writeb(addr, value, gb);

                case 0x0F:
                    gb->interrupts.if_reg = value;
                    return true;

                case 0x10 ... 0x14:
                case 0x16 ... 0x19:
                case 0x1A ... 0x1E:
                case 0x20 ... 0x23:
                case 0x24 ... 0x26:
                case 0x30 ... 0x3F:
                    return sound_reg_writeb(addr, value, gb);

                case 0x40 ... 0x4B:
                    return lcd_reg_writeb(addr, value, gb);

                case 0x50:
                    if ((value & 0x1)) {
                        gb->memory.bootrom_reg = 1;
                        logger(LOG_INFO, "Bootrom disabled");
                    }
                    return true;

                case 0x80 ... 0xFE: // HRAM
                    gb->memory.hram[addr - HRAM_LADDR] = value;
                    return true;

                case 0xFF:
                    gb->interrupts.ie_reg = value;
                    return true;
            }
    }
    logger(LOG_WARN, "mmu_writeb failed: value $%02X at address $%04X", value, addr);
    return false;
}
