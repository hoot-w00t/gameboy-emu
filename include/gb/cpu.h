/*
cpu.h
Header file for cpu.c

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

#ifndef _GB_CPU_H
#define _GB_CPU_H

void gb_cpu_push_stack(const uint16_t address, gb_system_t *gb);
uint16_t gb_cpu_pop_stack(gb_system_t *gb);
void gb_cpu_call(const uint16_t address, gb_system_t *gb);
void gb_cpu_ret(gb_system_t *gb);
int gb_cpu_cycle(gb_system_t *gb, const bool emulate_cycles);
void dump_cpu(gb_system_t *gb);

#endif