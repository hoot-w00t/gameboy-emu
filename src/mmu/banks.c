/*
banks.c
Memory bank functions

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
#include <string.h>

// Free memory banks
void membank_free(membank_t *mb)
{
    if (mb->banks) {
        for (uint16_t i = 0; i < mb->max_bank_nb; ++i) {
            free(mb->banks[i]);
        }
        free(mb->banks);
        mb->banks = NULL;
    }
}

// Initialize and allocate memory banks
void membank_init(uint16_t max_bank_nb, uint16_t bank_size, membank_t *mb)
{
    membank_free(mb);
    mb->max_bank_nb = max_bank_nb;
    mb->current_bank_nb = max_bank_nb;
    mb->bank_size = bank_size;
    mb->index = 0;
    mb->enabled = true;

    mb->banks = xalloc(sizeof(byte_t *) * max_bank_nb);
    for (uint16_t i = 0; i < max_bank_nb; ++i) {
        mb->banks[i] = xzalloc(bank_size);
    }
}

// Switch to bank #index
bool membank_switch(uint16_t index, membank_t *mb)
{
    if (!mb->enabled) {
        logger(LOG_ERROR, "membank_switch failed: memory bank disabled");
        return false;
    }

    if (index >= mb->current_bank_nb) {
        logger(LOG_ERROR,
               "membank_switch failed: bank #%u out of bounds (%u banks available)",
               index,
               mb->current_bank_nb);
        return false;
    }

    mb->index = index;
    return true;
}

// Increment bank index by 1
bool membank_inc(membank_t *mb)
{
    return membank_switch(mb->index + 1, mb);
}

// Read byte from memory bank
byte_t membank_readb(uint16_t addr, membank_t *mb)
{
    if (!mb->enabled) {
        logger(LOG_ERROR, "membank_readb failed: memory bank disabled");
        return 0;
    }

    if (mb->index >= mb->current_bank_nb || addr >= mb->bank_size) {
        logger(LOG_ERROR,
               "membank_readb failed: bank #%u out of bounds (%u banks available, $%04X)",
               mb->index,
               mb->current_bank_nb,
               addr);
        return 0;
    }

    return mb->banks[mb->index][addr];
}

// Write byte to memory bank
bool membank_writeb(uint16_t addr, byte_t value, membank_t *mb)
{
    if (!mb->enabled) {
        logger(LOG_ERROR, "membank_writeb failed: memory bank disabled");
        return false;
    }

    if (mb->index >= mb->current_bank_nb || addr >= mb->bank_size) {
        logger(LOG_ERROR,
               "membank_writeb failed: bank #%u out of bounds (%u banks available, $%04X)",
               mb->index,
               mb->current_bank_nb,
               addr);
        return false;
    }

    mb->banks[mb->index][addr] = value;
    return true;
}