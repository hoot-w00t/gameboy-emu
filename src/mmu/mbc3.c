/*
mbc3.c
MBC3 handler

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
#include "mmu/mbc3.h"
#include "mmu/banks.h"

#define mbc3_regs ((mbc3_regs_t *) gb->memory.mbc_regs)

void mbc3_clock(__attribute__((unused)) gb_system_t *gb)
{
    // TODO: Tick Real-Time Clock
}

int16_t mbc3_readb(uint16_t addr, gb_system_t *gb)
{
    if (addr <= ROM_BANK_0_UADDR) {
        return membank_readb_bank(addr, 0, &gb->memory.rom);

    } else if (ADDR_IN_RANGE(addr, ROM_BANK_N_LADDR, ROM_BANK_N_UADDR)) {
        return membank_readb(addr - ROM_BANK_N_LADDR, &gb->memory.rom);

    } else if (ADDR_IN_RANGE(addr, RAM_BANK_N_LADDR, RAM_BANK_N_UADDR)) {
        if (mbc3_regs->ram_bank <= 0x03) {
            return membank_readb(addr - RAM_BANK_N_LADDR, &gb->memory.ram);

        } else {
            switch (mbc3_regs->ram_bank) {
                case RTC_S:
                case RTC_M:
                case RTC_H:
                case RTC_DL:
                case RTC_DH:
                    logger(LOG_WARN, "mbc3_readb: address $%04X: RTC not implemented yet, returning 0", addr);
                    return 0;

                default:
                    logger(LOG_ERROR, "mbc3_readb: address $%04X: invalid register $%02X", addr, mbc3_regs->ram_bank);
                    return 0;
            }
        }
    } else {
        return -1;
    }
}

bool mbc3_writeb(uint16_t addr, byte_t value, gb_system_t *gb)
{
    if (addr <= 0x1FFF) {
        if ((mbc3_regs->ram_bank_enabled = ((value & 0xF) == 0xA))) {
            logger(LOG_DEBUG, "mbc3_writeb: RAM banking and RTC registers enabled");
        } else {
            logger(LOG_DEBUG, "mbc3_writeb: RAM banking and RTC registers disabled");
        }
        return true;

    } else if (ADDR_IN_RANGE(addr, 0x2000, 0x3FFF)) {
        logger(LOG_DEBUG, "mbc3_writeb: Switching to ROM bank $%02X", (value & 0x7F));
        membank_switch((value & 0x7F), &gb->memory.rom);
        return true;

    } else if (ADDR_IN_RANGE(addr, 0x4000, 0x5FFF)) {
        mbc3_regs->ram_bank = value;
        if (mbc3_regs->ram_bank <= 0x03) {
            logger(LOG_DEBUG, "mbc3_writeb: Switching to RAM bank $%02X", mbc3_regs->ram_bank);
            membank_switch(mbc3_regs->ram_bank, &gb->memory.ram);
        }
        return true;

    } else if (ADDR_IN_RANGE(addr, 0x6000, 0x7FFF)) {
        logger(LOG_WARN, "mbc3_writeb: address $%04X: Latch not implemented yet", addr);
        return true;

    } else if (ADDR_IN_RANGE(addr, RAM_BANK_N_LADDR, RAM_BANK_N_UADDR)) {
        if (!mbc3_regs->ram_bank_enabled) {
            logger(LOG_ERROR, "mbc3_writeb: address $%04X: RAM banks and RTC registers are disabled", addr);
            return false;
        }

        if (mbc3_regs->ram_bank <= 0x03)
            return membank_writeb(addr - RAM_BANK_N_LADDR, value, &gb->memory.ram);

        switch (mbc3_regs->ram_bank) {
            case RTC_S:
            case RTC_M:
            case RTC_H:
            case RTC_DL:
            case RTC_DH:
                logger(LOG_WARN, "mbc3_writeb: address $%04X: RTC not implemented yet", addr);
                return false;

            default:
                logger(LOG_ERROR, "mbc3_writeb: address $%04X: invalid register $%02X",
                    addr, mbc3_regs->ram_bank);
                return false;
        }
        return true;

    } else {
        return false;
    }
}