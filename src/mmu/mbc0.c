/*
mbc0.c
MBC0 (ROM only)

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
#include "mmu/banks.h"
#include "ppu/lcd_regs.h"

byte_t mbc0_readb(uint16_t addr, gb_system_t *gb)
{
    if (addr <= ROM_BANK_0_UADDR) {
        return membank_readb_bank(addr, 0, &gb->memory.rom);

    } else if (ADDR_IN_RANGE(addr, ROM_BANK_N_LADDR, ROM_BANK_N_UADDR)) {
        return membank_readb(addr - ROM_BANK_N_LADDR, &gb->memory.rom);

    } else if (ADDR_IN_RANGE(addr, VRAM_LADDR, VRAM_UADDR)) {
        if ((gb->screen.lcdc >> 7) && gb->screen.mode > LCDC_MODE_2) {
            logger(LOG_ERROR, "mbc0_readb failed: address $%04X: VRAM is not accessible", addr);
            return 0;
        }

        return gb->memory.vram[addr - VRAM_LADDR];

    } else if (ADDR_IN_RANGE(addr, RAM_BANK_0_LADDR, RAM_BANK_0_UADDR)) {
        return membank_readb_bank(addr - RAM_BANK_0_LADDR, 0, &gb->memory.ram);

    } else if (ADDR_IN_RANGE(addr, RAM_BANK_N_LADDR, RAM_BANK_N_UADDR)) {
        return membank_readb(addr - RAM_BANK_N_LADDR, &gb->memory.ram);

    } else if (ADDR_IN_RANGE(addr, OAM_LADDR, OAM_UADDR)) {
        if (gb->screen.dma_running || ((gb->screen.lcdc >> 7) && gb->screen.mode > LCDC_MODE_1)) {
            logger(LOG_ERROR, "mbc0_readb failed: address $%04X: OAM is not accessible", addr);
            return 0;
        }

        return gb->memory.oam[addr - OAM_LADDR];

    } else if (ADDR_IN_RANGE(addr, HRAM_LADDR, HRAM_UADDR)) {
        return gb->memory.hram[addr - HRAM_LADDR];

    } else if (ADDR_IN_RANGE(addr, LCDC, LCDC_WX)) {
        return lcd_reg_readb(addr, gb);

    } else if (addr == INTERRUPT_FLAG) {
        return gb->interrupts.if_reg;

    } else if (addr == INTERRUPT_ENABLE) {
        return gb->interrupts.ie_reg;

    } else {
        logger(LOG_ERROR, "mbc0_readb failed: address $%04X", addr);
        return 0;
    }
}

bool mbc0_writeb(uint16_t addr, byte_t value, gb_system_t *gb)
{
    if (ADDR_IN_RANGE(addr, ROM_BANK_0_LADDR, ROM_BANK_N_UADDR)) {
        logger(LOG_ERROR, "mbc0_writeb failed: address $%04X is read-only", addr);
        return false;

    } else if (ADDR_IN_RANGE(addr, VRAM_LADDR, VRAM_UADDR)) {
        if ((gb->screen.lcdc >> 7) && gb->screen.mode > LCDC_MODE_2) {
            logger(LOG_ERROR, "mbc0_writeb failed: address $%04X: VRAM is not accessible", addr);
            return false;
        }

        gb->memory.vram[addr - VRAM_LADDR] = value;
        return true;

    } else if (ADDR_IN_RANGE(addr, RAM_BANK_0_LADDR, RAM_BANK_0_UADDR)) {
        return membank_writeb_bank(addr - RAM_BANK_0_LADDR, value, 0, &gb->memory.ram);

    } else if (ADDR_IN_RANGE(addr, RAM_BANK_N_LADDR, RAM_BANK_N_UADDR)) {
        return membank_writeb(addr - RAM_BANK_N_LADDR, value, &gb->memory.ram);

    } else if (ADDR_IN_RANGE(addr, OAM_LADDR, OAM_UADDR)) {
        if (gb->screen.dma_running || ((gb->screen.lcdc >> 7) && gb->screen.mode > LCDC_MODE_1)) {
            logger(LOG_ERROR, "mbc0_writeb failed: address $%04X: OAM is not accessible", addr);
            return false;
        }

        gb->memory.oam[addr - OAM_LADDR] = value;
        return true;

    } else if (ADDR_IN_RANGE(addr, HRAM_LADDR, HRAM_UADDR)) {
        gb->memory.hram[addr - HRAM_LADDR] = value;
        return true;

    } else if (ADDR_IN_RANGE(addr, LCDC, LCDC_WX)) {
        return lcd_reg_writeb(addr, value, gb);

    } else if (addr == INTERRUPT_FLAG) {
        gb->interrupts.if_reg = value;
        return true;

    } else if (addr == INTERRUPT_ENABLE) {
        gb->interrupts.ie_reg = value;
        return true;

    } else {
        logger(LOG_ERROR, "mbc0_writeb failed: address $%04X", addr);
        return false;
    }
}