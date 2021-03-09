/*
registers.h
Function prototypes for registers.c

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

#ifndef _CPU_REGISTERS_H
#define _CPU_REGISTERS_H

#define reg_read_af(gb) ((gb->regs.a << 8) | gb->regs.f.data)
#define reg_read_bc(gb) ((gb->regs.b << 8) | gb->regs.c)
#define reg_read_de(gb) ((gb->regs.d << 8) | gb->regs.e)
#define reg_read_hl(gb) ((gb->regs.h << 8) | gb->regs.l)

static inline void reg_write_af(uint16_t value, gb_system_t *gb)
{
    gb->regs.a = (value >> 8);
    gb->regs.f.data = (value & 0xff);
}

static inline void reg_write_bc(uint16_t value, gb_system_t *gb)
{
    gb->regs.b = (value >> 8);
    gb->regs.c = (value & 0xff);
}

static inline void reg_write_de(uint16_t value, gb_system_t *gb)
{
    gb->regs.d = (value >> 8);
    gb->regs.e = (value & 0xff);
}

static inline void reg_write_hl(uint16_t value, gb_system_t *gb)
{
    gb->regs.h = (value >> 8);
    gb->regs.l = (value & 0xff);
}

#endif