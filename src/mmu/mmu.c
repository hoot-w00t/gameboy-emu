/*
mmu.c
Memory Management Unit

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

#include "logger.h"
#include "gameboy.h"
#include "mmu/mbc0.h"

// Read byte from addr
byte_t mmu_readb(uint16_t addr, gb_system_t *gb)
{
    if (gb->memory.readb_f) {
        return (*gb->memory.readb_f)(addr, gb);
    } else {
        logger(LOG_ERROR, "mmu_readb failed: no readb handler");
        return 0;
    }
}

// Write byte at addr
bool mmu_writeb(uint16_t addr, byte_t value, gb_system_t *gb)
{
    if (gb->memory.writeb_f) {
        return (*gb->memory.writeb_f)(addr, value, gb);
    } else {
        logger(LOG_ERROR, "mmu_writeb failed: no writeb handler");
        return false;
    }
}

// Set MBC type and initialize MBC-related functions and variables
bool mmu_set_mbc(byte_t mbc_type, gb_system_t *gb)
{
    switch (mbc_type) {
        case 0x00: // MBC0 (no MBC, ROM only)
            gb->memory.readb_f = &mbc0_readb;
            gb->memory.writeb_f = &mbc0_writeb;
            return true;

        default: logger(LOG_ERROR, "Unsupported MBC type $%02X", mbc_type);
    }
    return false;
}