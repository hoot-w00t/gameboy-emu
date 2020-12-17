/*
rombanks.h
Function prototypes for mmu/rombanks.c

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
#include "logger.h"

#ifndef _MMU_ROMBANKS_H
#define _MMU_ROMBANKS_H

void rombank_free(rombank_t *romb);
void rombank_alloc(uint16_t banks, rombank_t *romb);

// Switch $0000-$3FFF to bank
static inline bool rombank_switch_0(uint16_t bank, rombank_t *romb)
{
    if (bank < romb->banks_nb) {
        logger(LOG_DEBUG, "Switching $0000-$3FFF to ROM bank #%u ($%04X)", bank, bank);
        romb->bank_0 = bank;
        return true;
    } else {
        logger(LOG_ERROR,
            "Cannot switch $0000-$3FFF to ROM bank #%u ($%04X): Out of bounds, %u banks available",
            bank, bank, romb->banks_nb);
        return false;
    }
}

// Switch $4000-$7FFF to bank
static inline bool rombank_switch_n(uint16_t bank, rombank_t *romb)
{
    if (bank < romb->banks_nb) {
        logger(LOG_DEBUG, "Switching $4000-$7FFF to ROM bank #%u ($%04X)", bank, bank);
        romb->bank_n = bank;
        return true;
    } else {
        logger(LOG_ERROR,
            "Cannot switch $4000-$7FFF to ROM bank #%u ($%04X): Out of bounds, %u banks available",
            bank, bank, romb->banks_nb);
        return false;
    }
}

#endif