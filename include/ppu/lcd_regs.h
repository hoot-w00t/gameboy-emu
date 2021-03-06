/*
lcd_regs.h
Function prototypes for lcd_regs.c

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

#ifndef _PPU_LCD_REGS_H
#define _PPU_LCD_REGS_H

byte_t lcd_reg_readb(uint16_t addr, gb_system_t *gb);
bool lcd_reg_writeb(uint16_t addr, byte_t value, gb_system_t *gb);

#endif