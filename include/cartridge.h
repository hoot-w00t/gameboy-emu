/*
cartridge.h
Header and function prototypes for cartridge.c

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

#ifndef _CARTRIDGE_H
#define _CARTRIDGE_H

char *cartridge_publisher(cartridge_hdr_t *cr);
bool cartridge_decode_hdr(byte_t *data, cartridge_hdr_t *cr);
byte_t compute_header_checksum(byte_t *data);
bool cartridge_check_logo(cartridge_hdr_t *cr);
void cartridge_dump(cartridge_hdr_t *cr);

#endif