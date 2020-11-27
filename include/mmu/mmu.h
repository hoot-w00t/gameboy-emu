/*
mmu.h
Function prototypes for mmu/mmu.c

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

#ifndef _MMU_MMU_H
#define _MMU_MMU_H

byte_t mmu_bootrom_readb(byte_t addr, gb_system_t *gb);
byte_t mmu_readb(uint16_t addr, gb_system_t *gb);
byte_t mmu_readb_nolog(uint16_t addr, gb_system_t *gb);
bool mmu_writeb(uint16_t addr, byte_t value, gb_system_t *gb);
uint16_t mmu_read_u16(uint16_t addr, gb_system_t *gb);
bool mmu_write_u16(uint16_t addr, uint16_t value, gb_system_t *gb);
bool mmu_oam_blocked(gb_system_t *gb);
bool mmu_vram_blocked(gb_system_t *gb);
bool mmu_set_mbc(byte_t mbc_type, gb_system_t *gb);
void mmu_dump(uint16_t addr, uint16_t n, gb_system_t *gb);

#endif