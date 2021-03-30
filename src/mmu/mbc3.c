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
#include "mmu/rombanks.h"
#include "mmu/rambanks.h"

#define mbc3_regs ((mbc3_regs_t *) gb->memory.mbc_regs)

void mbc3_clock(__attribute__((unused)) gb_system_t *gb)
{
    // TODO: Tick Real-Time Clock
}

int16_t mbc3_readb(uint16_t addr, gb_system_t *gb)
{
    if (ADDR_IN_RANGE(addr, RAM_BANK_N_LADDR, RAM_BANK_N_UADDR)) {
        if (mbc3_regs->ram_bank <= 0x03)
            return -1; // Let mmu_internal handle it

        switch (mbc3_regs->ram_bank) {
            case RTC_S:
            case RTC_M:
            case RTC_H:
            case RTC_DL:
            case RTC_DH:
                logger(LOG_WARN, "mbc3_readb: address $%04X: RTC not implemented yet, returning 0", addr);
                return MMU_UNMAPPED_ADDR_VALUE;

            default:
                logger(LOG_ERROR, "mbc3_readb: address $%04X: invalid register $%02X", addr, mbc3_regs->ram_bank);
                return MMU_UNMAPPED_ADDR_VALUE;
        }
    } else {
        return -1;
    }
}

bool mbc3_writeb(uint16_t addr, byte_t value, gb_system_t *gb)
{
    switch (addr >> 12) {
        case 0x0:
        case 0x1:
            if ((gb->memory.ram.can_write = ((value & 0xF) == 0xA))) {
                logger(LOG_DEBUG, "mbc3: RAM banking and RTC registers enabled");
            } else {
                logger(LOG_DEBUG, "mbc3: RAM banking and RTC registers disabled");
            }
            gb->memory.ram.can_read = gb->memory.ram.can_write;
            return true;

        case 0x2:
        case 0x3:
            rombank_switch_n((value & 0x7F), &gb->memory.rom);
            return true;

        case 0x4:
        case 0x5:
            mbc3_regs->ram_bank = value;
            if (mbc3_regs->ram_bank <= 0x03)
                rambank_switch(mbc3_regs->ram_bank, &gb->memory.ram);
            return true;

        case 0x6:
        case 0x7:
            logger(LOG_WARN, "mbc3_writeb: address $%04X: Latch not implemented yet", addr);
            return true;

        case 0xA:
        case 0xB:
            if (mbc3_regs->ram_bank <= 0x03)
                return false; // Let mmu_internal handle it

            switch (mbc3_regs->ram_bank) {
                case RTC_S:
                case RTC_M:
                case RTC_H:
                case RTC_DL:
                case RTC_DH:
                    logger(LOG_WARN, "mbc3_writeb: address $%04X: RTC not implemented yet", addr);
                    break;

                default:
                    logger(LOG_ERROR, "mbc3_writeb: address $%04X: invalid register $%02X",
                        addr, mbc3_regs->ram_bank);
                    break;
            }
            return true;

        default: return false;
    }
}