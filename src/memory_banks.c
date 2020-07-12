/*
memory_banks.c
GameBoy memory banks access functions

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

#include "gb/defs.h"
#include "logger.h"
#include <stdlib.h>
#include <string.h>

// Free dynamically allocated membank memory
void gb_free_membank(gb_membank_t *membank)
{
    if (membank->banks) {
        for (byte_t i = 0; i < membank->maxsize; ++i) {
            free(membank->banks[i]);
        }
        free(membank->banks);
    }
}

// Dynamically allocate memory for the banks
bool gb_allocate_membank(gb_membank_t *membank)
{
    membank->banks = malloc(sizeof(byte_t *) * membank->maxsize);
    if (!membank->banks) {
        logger(LOG_CRIT, "Memory allocation failed");
        return false;
    }

    memset(membank->banks, 0, sizeof(byte_t *) * membank->maxsize);
    for (byte_t i = 0; i < membank->maxsize; ++i) {
        membank->banks[i] = malloc(membank->bank_size);
        if (!membank->banks[i]) {
            logger(LOG_CRIT, "Memory allocation failed");
            gb_free_membank(membank);
            return false;
        }

        memset(membank->banks[i], 0, membank->bank_size);
    }

    return true;
}

// Switch to bank at index
bool gb_switch_membank(byte_t index, gb_membank_t *membank)
{
    if (index >= membank->maxsize) {
        logger(
            LOG_CRIT,
            "Unable to switch to memory bank #%u: only %u banks available",
            index + 1,
            membank->maxsize
        );
        return false;
    }

    membank->index = index;
    return true;
}

// Increment bank index by 1
bool gb_increment_membank(gb_membank_t *membank)
{
    if (membank->index + 1 >= membank->maxsize) {
        logger(
            LOG_CRIT,
            "Unable to increment to memory bank #%u: only %u banks available",
            membank->index + 2,
            membank->maxsize
        );
        return false;
    }

    membank->index += 1;
    return true;
}

// Read byte at address from selected memory bank
byte_t gb_read_byte_from_membank(uint16_t address, gb_membank_t *membank)
{
    if (address >= membank->bank_size) {
        logger(
            LOG_CRIT,
            "Unable to read from memory bank, address 0x%04X is out of bounds",
            address
        );
        return 0;
    }

    if (membank->index < membank->maxsize) {
        return membank->banks[membank->index][address];
    } else {
        logger(
            LOG_CRIT,
            "Unable to read from memory bank #%u: only %u banks available",
            membank->index + 1,
            membank->maxsize
        );
        return 0;
    }
}

// Write byte at address to selected memory bank
bool gb_write_byte_to_membank(uint16_t address, byte_t value, gb_membank_t *membank)
{
    if (address >= membank->bank_size) {
        logger(
            LOG_CRIT,
            "Unable to write to memory bank, address 0x%04X is out of bounds",
            address
        );
        return false;
    }

    if (membank->index < membank->maxsize) {
        membank->banks[membank->index][address] = value;
        return true;
    } else {
        logger(
            LOG_CRIT,
            "Unable to write to memory bank #%u: only %u banks available",
            membank->index + 1,
            membank->maxsize
        );
        return false;
    }
}