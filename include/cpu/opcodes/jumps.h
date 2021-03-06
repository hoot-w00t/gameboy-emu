/*
jumps.h
Function prototypes for jumps.c

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

#ifndef _CPU_OPCODE_JUMPS_H
#define _CPU_OPCODE_JUMPS_H

int opcode_jp_nn(const opcode_t *opcode, gb_system_t *gb);
int opcode_jp_nz_nn(const opcode_t *opcode, gb_system_t *gb);
int opcode_jp_z_nn(const opcode_t *opcode, gb_system_t *gb);
int opcode_jp_nc_nn(const opcode_t *opcode, gb_system_t *gb);
int opcode_jp_c_nn(const opcode_t *opcode, gb_system_t *gb);
int opcode_jp_hl(const opcode_t *opcode, gb_system_t *gb);
int opcode_jr_n(const opcode_t *opcode, gb_system_t *gb);
int opcode_jr_nz_n(const opcode_t *opcode, gb_system_t *gb);
int opcode_jr_z_n(const opcode_t *opcode, gb_system_t *gb);
int opcode_jr_nc_n(const opcode_t *opcode, gb_system_t *gb);
int opcode_jr_c_n(const opcode_t *opcode, gb_system_t *gb);

#endif