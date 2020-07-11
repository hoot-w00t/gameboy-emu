/*
cartridge.h
Header file for cartridge.c

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

#include "gb_defs.h"

#ifndef _GB_CARTRIDGE_H
#define _GB_CARTRIDGE_H

void decode_cartridge_header(byte_t *data, gb_cartridge_hdr_t *cartridge);
byte_t compute_header_checksum(byte_t *data);
uint16_t compute_global_checksum(byte_t *data);
void dump_cartridge_header(gb_cartridge_hdr_t *cartridge);

#endif