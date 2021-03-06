/*
timer.h
Function prototypes for timer.c

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

#ifndef _TIMER_H
#define _TIMER_H

byte_t timer_reg_readb(uint16_t addr, gb_system_t *gb);
bool timer_reg_writeb(uint16_t addr, byte_t value, gb_system_t *gb);
void timer_cycle(gb_system_t *gb);

#endif