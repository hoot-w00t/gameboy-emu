/*
rambanks.c
Cartridge RAM banks

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

// Free allocated RAM banks
void rambank_free(rambank_t *ramb)
{
    if (ramb->banks) {
        logger(LOG_DEBUG, "rambank_free: Freeing %u banks", ramb->banks_nb);
        for (uint16_t i = 0; i < ramb->banks_nb; ++i)
            free(ramb->banks[i]);
        free(ramb->banks);
    }
}

// Allocate RAM banks of bank_size
void rambank_alloc(uint16_t banks, uint16_t bank_size, rambank_t *ramb)
{
    ramb->bank = 0;
    if (banks == 0) {
        ramb->banks_nb = 0;
        ramb->bank_size = 0;
        ramb->banks = NULL;
    } else {
        ramb->banks_nb = banks;
        logger(LOG_DEBUG, "rambank_alloc: Allocating %u banks", ramb->banks_nb);
        ramb->bank_size = bank_size;
        ramb->banks = xalloc(sizeof(byte_t *) * ramb->banks_nb);
        for (uint16_t i = 0; i < ramb->banks_nb; ++i)
            ramb->banks[i] = xzalloc(bank_size);
    }
}