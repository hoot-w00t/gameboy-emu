/*
rombanks.c
Cartridge ROM and ROM banking

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

#include "xalloc.h"
#include "logger.h"
#include "gameboy.h"
#include <stdlib.h>

// Free allocated ROM banks
void rombank_free(rombank_t *romb)
{
    if (romb->banks) {
        logger(LOG_DEBUG, "rombank_free: Freeing %u banks", romb->banks_nb);
        for (uint16_t i = 0; i < romb->banks_nb; ++i)
            free(romb->banks[i]);
        free(romb->banks);
        romb->banks = NULL;
    }
}

// Allocate ROM banks of ROM_BANK_SIZE
void rombank_alloc(uint16_t banks, rombank_t *romb)
{
    if (banks < 2) {
        logger(LOG_CRIT, "rombank_alloc: Cannot allocate less than 2 ROM banks");
        abort();
    }

    romb->banks_nb = banks;
    logger(LOG_DEBUG, "rombank_alloc: Allocating %u banks", romb->banks_nb);
    romb->banks = xalloc(sizeof(byte_t *) * romb->banks_nb);
    for (uint16_t i = 0; i < romb->banks_nb; ++i)
        romb->banks[i] = xalloc(ROM_BANK_SIZE);
    romb->bank_0 = 0;
    romb->bank_n = 1;
}