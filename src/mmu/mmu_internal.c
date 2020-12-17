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
    if (addr <= ROM_BANK_0_UADDR) {
        return gb->memory.rom.banks[gb->memory.rom.bank_0][addr];

    } else if (ADDR_IN_RANGE(addr, ROM_BANK_N_LADDR, ROM_BANK_N_UADDR)) {
        return gb->memory.rom.banks[gb->memory.rom.bank_n][addr - ROM_BANK_N_LADDR];

    } else if (ADDR_IN_RANGE(addr, VRAM_LADDR, VRAM_UADDR)) {
        if (mmu_vram_blocked(gb)) {
            logger(LOG_ERROR, "mmu_readb failed: address $%04X: VRAM is not accessible", addr);
            return 0xFF;
        }

        return gb->memory.vram[addr - VRAM_LADDR];

    } else if (ADDR_IN_RANGE(addr, RAM_BANK_0_LADDR, RAM_BANK_0_UADDR)) {
        return gb->memory.wram[addr - RAM_BANK_0_LADDR];

    } else if (ADDR_IN_RANGE(addr, RAM_ECHO_LADDR, RAM_ECHO_UADDR)) {
        return gb->memory.wram[addr - RAM_ECHO_LADDR];

    } else if (ADDR_IN_RANGE(addr, RAM_BANK_N_LADDR, RAM_BANK_N_UADDR)) {
        if (!rambank_exists(&gb->memory.ram)) {
            logger(LOG_ERROR, "mmu_readb failed: address $%04X: No RAM banks are available", addr);
            return 0;
        }
        return gb->memory.ram.banks[gb->memory.ram.bank][addr - RAM_BANK_N_LADDR];

    } else if (ADDR_IN_RANGE(addr, OAM_LADDR, OAM_UADDR)) {
        if (mmu_oam_blocked(gb)) {
            logger(LOG_ERROR, "mmu_readb failed: address $%04X: OAM is not accessible", addr);
            return 0;
        }

        return gb->memory.oam[addr - OAM_LADDR];

    } else if (ADDR_IN_RANGE(addr, HRAM_LADDR, HRAM_UADDR)) {
        return gb->memory.hram[addr - HRAM_LADDR];

    } else if (ADDR_IN_RANGE(addr, TIM_DIV, TIM_TAC)) {
        return timer_reg_readb(addr, gb);

    } else if (ADDR_IN_RANGE(addr, LCDC, LCDC_WX)) {
        return lcd_reg_readb(addr, gb);

    } else if (   ADDR_IN_RANGE(addr, SOUND_NR10, SOUND_NR14)
               || ADDR_IN_RANGE(addr, SOUND_NR21, SOUND_NR24)
               || ADDR_IN_RANGE(addr, SOUND_NR30, SOUND_NR34)
               || ADDR_IN_RANGE(addr, SOUND_NR41, SOUND_NR44)
               || ADDR_IN_RANGE(addr, SOUND_NR50, SOUND_NR52)
               || ADDR_IN_RANGE(addr, SOUND_WAVE_PATTERN_LADDR,
                      SOUND_WAVE_PATTERN_UADDR)) {
        return sound_reg_readb(addr, gb);

    } else if (addr == JOYPAD_REG) {
        return joypad_reg_readb(gb);

    } else if (addr == SERIAL_SB || addr == SERIAL_SC) {
        return serial_reg_readb(addr, gb);

    } else if (addr == BOOTROM_REG_ADDR) {
        return gb->memory.bootrom_reg;

    } else if (addr == INTERRUPT_FLAG) {
        return gb->interrupts.if_reg;

    } else if (addr == INTERRUPT_ENABLE) {
        return gb->interrupts.ie_reg;

    } else {
        logger(LOG_WARN, "mmu_readb failed: address $%04X", addr);
        return 0;
    }
}

bool mmu_internal_writeb(uint16_t addr, byte_t value, gb_system_t *gb)
{
    if (addr <= ROM_BANK_N_UADDR) {
        logger(LOG_ERROR, "mmu_writeb failed: address $%04X is read-only", addr);
        return false;

    } else if (ADDR_IN_RANGE(addr, VRAM_LADDR, VRAM_UADDR)) {
        if (mmu_vram_blocked(gb)) {
            logger(LOG_ERROR, "mmu_writeb failed: address $%04X: VRAM is not accessible", addr);
            return false;
        }

        gb->memory.vram[addr - VRAM_LADDR] = value;
        return true;

    } else if (ADDR_IN_RANGE(addr, RAM_BANK_0_LADDR, RAM_BANK_0_UADDR)) {
        gb->memory.wram[addr - RAM_BANK_0_LADDR] = value;
        return true;

    } else if (ADDR_IN_RANGE(addr, RAM_ECHO_LADDR, RAM_ECHO_UADDR)) {
        gb->memory.wram[addr - RAM_ECHO_LADDR] = value;
        return true;

    } else if (ADDR_IN_RANGE(addr, RAM_BANK_N_LADDR, RAM_BANK_N_UADDR)) {
        if (!rambank_exists(&gb->memory.ram)) {
            logger(LOG_ERROR, "mmu_writeb failed: address $%04X: No RAM banks are available", addr);
            return false;
        }
        gb->memory.ram.banks[gb->memory.ram.bank][addr - RAM_BANK_N_LADDR] = value;
        return true;

    } else if (ADDR_IN_RANGE(addr, OAM_LADDR, OAM_UADDR)) {
        if (mmu_oam_blocked(gb)) {
            logger(LOG_ERROR, "mmu_writeb failed: address $%04X: OAM is not accessible", addr);
            return false;
        }

        gb->memory.oam[addr - OAM_LADDR] = value;
        return true;

    } else if (ADDR_IN_RANGE(addr, HRAM_LADDR, HRAM_UADDR)) {
        gb->memory.hram[addr - HRAM_LADDR] = value;
        return true;

    } else if (ADDR_IN_RANGE(addr, TIM_DIV, TIM_TAC)) {
        return timer_reg_writeb(addr, value, gb);

    } else if (ADDR_IN_RANGE(addr, LCDC, LCDC_WX)) {
        return lcd_reg_writeb(addr, value, gb);

    } else if (   ADDR_IN_RANGE(addr, SOUND_NR10, SOUND_NR14)
               || ADDR_IN_RANGE(addr, SOUND_NR21, SOUND_NR24)
               || ADDR_IN_RANGE(addr, SOUND_NR30, SOUND_NR34)
               || ADDR_IN_RANGE(addr, SOUND_NR41, SOUND_NR44)
               || ADDR_IN_RANGE(addr, SOUND_NR50, SOUND_NR52)
               || ADDR_IN_RANGE(addr, SOUND_WAVE_PATTERN_LADDR,
                      SOUND_WAVE_PATTERN_UADDR)) {
        return sound_reg_writeb(addr, value, gb);

    } else if (addr == JOYPAD_REG) {
        return joypad_reg_writeb(value, gb);
    } else if (addr == SERIAL_SB || addr == SERIAL_SC) {
        return serial_reg_writeb(addr, value, gb);

    } else if (addr == BOOTROM_REG_ADDR) {
        gb->memory.bootrom_reg = value;
        if (value) {
            logger(LOG_INFO, "Bootrom disabled");
        } else {
            logger(LOG_WARN, "Bootrom enabled");
        }
        return true;

    } else if (addr == INTERRUPT_FLAG) {
        gb->interrupts.if_reg = value;
        return true;

    } else if (addr == INTERRUPT_ENABLE) {
        gb->interrupts.ie_reg = value;
        return true;

    } else {
        logger(LOG_WARN, "mmu_writeb failed: value $%02X at address $%04X", value, addr);
        return false;
    }
}