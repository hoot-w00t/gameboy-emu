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
#include <string.h>

#define mbc3_regs ((mbc3_regs_t *) gb->memory.mbc_regs)

void mbc3_rtc_tick_timestamp(gb_system_t *gb)
{
    time_t current_tick = time(NULL);
    time_t elapsed = current_tick - mbc3_regs->last_tick;
    time_t total_d;

    if (elapsed > 0) {
        logger(LOG_ALL, "mbc3: Ticking %li seconds", elapsed);
        total_d = elapsed / 60 / 60 / 24;
        if ((mbc3_regs->rtc.rtc_s += (elapsed % 60)) >= 60) {
            mbc3_regs->rtc.rtc_m += mbc3_regs->rtc.rtc_s / 60;
            mbc3_regs->rtc.rtc_s %= 60;
        }
        if ((mbc3_regs->rtc.rtc_m += (elapsed / 60 % 60)) >= 60) {
            mbc3_regs->rtc.rtc_h += mbc3_regs->rtc.rtc_m / 60;
            mbc3_regs->rtc.rtc_m %= 60;
        }
        if ((mbc3_regs->rtc.rtc_h += (elapsed / 60 / 60 % 24)) >= 24) {
            total_d += mbc3_regs->rtc.rtc_h / 24;
            mbc3_regs->rtc.rtc_h %= 24;
        }
        total_d += mbc3_regs->rtc.rtc_dl | (mbc3_regs->rtc.rtc_dh.d.upper_bit << 8);
        mbc3_regs->rtc.rtc_dl = total_d & 0xFF;
        mbc3_regs->rtc.rtc_dh.d.upper_bit = (total_d >> 8) & 0x1;
        mbc3_regs->rtc.rtc_dh.d.carry = (total_d % 512) & 0x1;
    }
    mbc3_regs->last_tick = current_tick;
}

static void mbc3_rtc_tick(gb_system_t *gb)
{
    mbc3_regs->last_tick = time(NULL);
    if ((mbc3_regs->rtc.rtc_s += 1) >= 60) {
        mbc3_regs->rtc.rtc_s = 0;
        if ((mbc3_regs->rtc.rtc_m += 1) >= 60) {
            mbc3_regs->rtc.rtc_m = 0;
            if ((mbc3_regs->rtc.rtc_h += 1) >= 24) {
                mbc3_regs->rtc.rtc_h = 0;
                if ((mbc3_regs->rtc.rtc_dl += 1) == 0) {
                    if (mbc3_regs->rtc.rtc_dh.d.upper_bit)
                        mbc3_regs->rtc.rtc_dh.d.carry = 1;
                    mbc3_regs->rtc.rtc_dh.d.upper_bit ^= 1;
                }
            }
        }
    }
}

void mbc3_clock(gb_system_t *gb)
{
    if (!mbc3_regs->rtc.rtc_dh.d.halt && (mbc3_regs->clocks += 1) >= CPU_CLOCK_SPEED) {
        mbc3_regs->clocks = 0;
        mbc3_rtc_tick(gb);
    }
}

int16_t mbc3_readb(uint16_t addr, gb_system_t *gb)
{
    if (ADDR_IN_RANGE(addr, RAM_BANK_N_LADDR, RAM_BANK_N_UADDR)) {
        switch (mbc3_regs->ram_bank) {
            case 0x0:
            case 0x1:
            case 0x2:
            case 0x3:
                return -1; // Let mmu_internal handle it

            case RTC_S : return mbc3_regs->latch.rtc_s;
            case RTC_M : return mbc3_regs->latch.rtc_m;
            case RTC_H : return mbc3_regs->latch.rtc_h;
            case RTC_DL: return mbc3_regs->latch.rtc_dl;
            case RTC_DH: return mbc3_regs->latch.rtc_dh.b;
            default:
                logger(LOG_ERROR, "mbc3_readb: $%04X: invalid RTC $%02X", addr, mbc3_regs->ram_bank);
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
            rombank_switch_n((value & 0x7F) ?: 0x01, &gb->memory.rom);
            return true;

        case 0x4:
        case 0x5:
            mbc3_regs->ram_bank = value;
            if (mbc3_regs->ram_bank <= 0x03)
                rambank_switch(mbc3_regs->ram_bank, &gb->memory.ram);
            return true;

        case 0x6:
        case 0x7:
            value &= 0x1;
            if (mbc3_regs->latch_reg == 0x0 && value) {
                mbc3_regs->latch.rtc_s = mbc3_regs->rtc.rtc_s & 0x3F;
                mbc3_regs->latch.rtc_m = mbc3_regs->rtc.rtc_m & 0x3F;
                mbc3_regs->latch.rtc_h = mbc3_regs->rtc.rtc_h & 0x1F;
                mbc3_regs->latch.rtc_dl = mbc3_regs->rtc.rtc_dl;
                mbc3_regs->latch.rtc_dh.b = mbc3_regs->rtc.rtc_dh.b & 0xC1;

                logger(LOG_ALL, "mbc3: Latched RTC: day %u, %02u:%02u:%02u (halt=%u, carry=%u)",
                    mbc3_regs->latch.rtc_dl | (mbc3_regs->latch.rtc_dh.d.upper_bit << 8),
                    mbc3_regs->latch.rtc_h,
                    mbc3_regs->latch.rtc_m,
                    mbc3_regs->latch.rtc_s,
                    mbc3_regs->latch.rtc_dh.d.halt,
                    mbc3_regs->latch.rtc_dh.d.carry);
            }
            mbc3_regs->latch_reg = value;
            return true;

        case 0xA:
        case 0xB:
            if (mbc3_regs->ram_bank <= 0x03)
                return false; // Let mmu_internal handle it

            if (gb->memory.ram.can_write) {
                logger(LOG_ALL, "mbc3_writeb: $%02X to RTC $%02X", value, mbc3_regs->ram_bank);
                switch (mbc3_regs->ram_bank) {
                    case RTC_S : mbc3_regs->rtc.rtc_s = value; break;
                    case RTC_M : mbc3_regs->rtc.rtc_m = value; break;
                    case RTC_H : mbc3_regs->rtc.rtc_h = value; break;
                    case RTC_DL: mbc3_regs->rtc.rtc_dl = value; break;
                    case RTC_DH:
                        mbc3_regs->rtc.rtc_dh.b = value;
                        if (mbc3_regs->rtc.rtc_dh.d.halt)
                            mbc3_regs->clocks = 0;
                        break;
                    default:
                        logger(LOG_ERROR, "mbc3_writeb: $%04X: invalid RTC $%02X", addr, mbc3_regs->ram_bank);
                        break;
                }
            } else {
                logger(LOG_ERROR, "mbc3_writeb failed: $%02X to RTC $%02X: Writing is disabled", value, mbc3_regs->ram_bank);
            }
            return true;

        default: return false;
    }
}