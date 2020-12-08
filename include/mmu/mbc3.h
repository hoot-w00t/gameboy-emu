/*
mbc3.h
Function prototypes for mmu/mbc3.c

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

#include "gameboy.h"

#ifndef _MMU_MBC3_H
#define _MMU_MBC3_H

#define RTC_S  (0x08)
#define RTC_M  (0x09)
#define RTC_H  (0x0A)
#define RTC_DL (0x0B)
#define RTC_DH (0x0C)

#define RTC_CLOCK (32768)
#define RTC_CLOCKS_PER_SECOND (CPU_CLOCK_SPEED / RTC_CLOCK)

typedef struct mbc3_regs mbc3_regs_t;

struct rtc_regs {
    byte_t rtc_s;
    byte_t rtc_m;
    byte_t rtc_h;
    uint16_t rtc_d : 9;
    byte_t halt_flag : 1;
    byte_t day_counter_carry : 1;
};

struct mbc3_regs {
    struct rtc_regs rtc;
    struct rtc_regs rtc_latched;
    byte_t latch_reg;
    byte_t ram_bank;
    bool ram_bank_enabled;
    uint16_t clocks;
    uint16_t clocks_per_second;
};

void mbc3_clock(gb_system_t *gb);
int16_t mbc3_readb(uint16_t addr, gb_system_t *gb);
bool mbc3_writeb(uint16_t addr, byte_t value, gb_system_t *gb);

#endif