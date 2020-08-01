/*
mbc.c
GameBoy memory bank controller

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

#include "gb/mbc.h"
#include "logger.h"
#include <stddef.h>

bool gb_mbc_romonly_readb(__attribute__((unused)) uint16_t address, __attribute__((unused)) gb_system_t *gb)
{
    return false; // ROM Only does not have a MBC, so we always return false
}

bool gb_mbc_romonly_writeb(__attribute__((unused)) uint16_t address, __attribute__((unused)) byte_t value, __attribute__((unused)) gb_system_t *gb)
{
    return false; // ROM Only does not have a MBC, so we always return false
}

mbc_readb_t get_mbc_readb_fptr(gb_cartridge_hdr_t *cartridge)
{
    switch (cartridge->mbc_type) {
        case 0x00: return &gb_mbc_romonly_readb;
        default:
            logger(LOG_CRIT, "Unsupported MBC type (for reading): 0x%02X", cartridge->mbc_type);
            return NULL;
    }
}

mbc_writeb_t get_mbc_writeb_fptr(gb_cartridge_hdr_t *cartridge)
{
    switch (cartridge->mbc_type) {
        case 0x00: return &gb_mbc_romonly_writeb;
        default:
            logger(LOG_CRIT, "Unsupported MBC type (for writing): 0x%02X", cartridge->mbc_type);
            return NULL;
    }
}