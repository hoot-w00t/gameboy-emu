/*
add.h
Function prototypes for add.c

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

#ifndef _CPU_OPCODE_ALU_ADD_H
#define _CPU_OPCODE_ALU_ADD_H

byte_t cpu_addb(const byte_t target, const byte_t value, gb_system_t *gb);
uint16_t cpu_add_u16(const uint16_t target, const uint16_t value, gb_system_t *gb);
uint16_t cpu_add_sp_e(const sbyte_t e, gb_system_t *gb);
int opcode_add_a_n(const opcode_t *opcode, gb_system_t *gb);
int opcode_add_hl_n(const opcode_t *opcode, gb_system_t *gb);
int opcode_add_sp_n(const opcode_t *opcode, gb_system_t *gb);

#endif