/*
banks.h
Function prototypes for mmu/banks.c

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

#ifndef _MMU_BANKS_H
#define _MMU_BANKS_H

void membank_free(membank_t *mb);
void membank_init(uint16_t max_bank_nb, uint16_t bank_size, membank_t *mb);
bool membank_switch(uint16_t index, membank_t *mb);
bool membank_inc(membank_t *mb);
byte_t membank_readb(uint16_t addr, membank_t *mb);
bool membank_writeb(uint16_t addr, byte_t value, membank_t *mb);

#endif