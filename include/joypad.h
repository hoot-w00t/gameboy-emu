/*
joypad.h
Function prototypes for joypad.c

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

#ifndef _JOYPAD_H
#define _JOYPAD_H

byte_t joypad_reg_readb(gb_system_t *gb);
bool joypad_reg_writeb(const byte_t value, gb_system_t *gb);
void joypad_button(const byte_t button, const bool pressed, gb_system_t *gb);

#endif