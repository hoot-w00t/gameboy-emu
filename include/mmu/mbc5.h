/*
mbc5.h
Function prototypes for mmu/mbc5.c

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

#ifndef _MMU_MBC5_H
#define _MMU_MBC5_H

typedef struct mbc5_regs mbc5_regs_t;

struct mbc5_regs {
    byte_t rom_bank_lo;
    byte_t rom_bank_hi;
};

bool mbc5_writeb(uint16_t addr, byte_t value, gb_system_t *gb);

#endif