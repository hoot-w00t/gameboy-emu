/*
ld.h
Function prototypes for ld.c

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

#ifndef _CPU_OPCODE_LD_H
#define _CPU_OPCODE_LD_H

void cpu_pushb(byte_t value, gb_system_t *gb);
void cpu_push_u16(uint16_t value, gb_system_t *gb);
byte_t cpu_popb(byte_t value, gb_system_t *gb);
uint16_t cpu_pop_u16(uint16_t value, gb_system_t *gb);
int opcode_push(const opcode_t *opcode, gb_system_t *gb);
int opcode_pop(const opcode_t *opcode, gb_system_t *gb);

#endif