/*
interrupts.h
Function prototypes for interrupts.c

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

#ifndef _CPU_INTERRUPTS_H
#define _CPU_INTERRUPTS_H

bool cpu_int_flag(byte_t int_bit, gb_system_t *gb);
void cpu_int_flag_set(byte_t int_bit, gb_system_t *gb);
void cpu_int_flag_clear(byte_t int_bit, gb_system_t *gb);
int cpu_int_isr(gb_system_t *gb);

#endif