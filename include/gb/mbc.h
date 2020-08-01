/*
mbc.h
Header file for MBC function prototypes

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

#ifndef _GB_MBC_H
#define _GB_MBC_H

// ROM Only (MBC Type 0x00)
bool gb_mbc_romonly_readb(uint16_t address, gb_system_t *gb);
bool gb_mbc_romonly_writeb(uint16_t address, byte_t value, gb_system_t *gb);

// MBC3 (Types 0x)
bool gb_mbc_3_readb(uint16_t address, gb_system_t *gb);
bool gb_mbc_3_writeb(uint16_t address, byte_t value, gb_system_t *gb);

// Get function pointers from the MBC type
mbc_readb_t get_mbc_readb_fptr(gb_cartridge_hdr_t *cartridge);
mbc_writeb_t get_mbc_writeb_fptr(gb_cartridge_hdr_t *cartridge);

#endif