/*
interrupts.h
Header file for interrupts.c

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

#ifndef _GB_INTERRUPTS_H
#define _GB_INTERRUTPS_H

void set_interrupt_bit(byte_t interrupt, byte_t *dest, bool enable);
void gb_set_interrupt_request(byte_t interrupt, bool enable, gb_system_t *gb);
void gb_enable_interrupts(gb_system_t *gb);
void gb_disable_interrupts(gb_system_t *gb);
void gb_execute_interrupt(byte_t interrupt, gb_system_t *gb);

#endif