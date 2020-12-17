/*
rambanks.h
Function prototypes for mmu/rambanks.c

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

#ifndef _MMU_RAMBANKS_H
#define _MMU_RAMBANKS_H

#define rambank_exists(ramb) ((ramb)->banks_nb > 0)

void rambank_free(rambank_t *ramb);
void rambank_alloc(uint16_t banks, uint16_t bank_size, rambank_t *ramb);

// Switch $A000-$BFFF to bank
static inline bool rambank_switch(uint16_t bank, rambank_t *ramb)
{
    if (bank < ramb->banks_nb) {
        logger(LOG_DEBUG, "Switching $A000-$BFFF to RAM bank #%u ($%04X)", bank, bank);
        ramb->bank = bank;
        return true;
    } else {
        logger(LOG_ERROR,
            "Cannot switch $A000-$BFFF to RAM bank #%u ($%04X): Out of bounds, %u banks available",
            bank, bank, ramb->banks_nb);
        return false;
    }
}

#endif