/*
control.c
CPU Control operations

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

#include "gb/opcodes.h"

// NOP instruction (0x00)
int gb_opcode_nop(__attribute__((unused)) const gb_opcode_t *opcode, __attribute__((unused)) gb_system_t *gb)
{
    return OPCODE_ACTION;
}

// STOP instruction (0x10)
int gb_opcode_stop(__attribute__((unused)) const gb_opcode_t *opcode, __attribute__((unused)) gb_system_t *gb)
{
    return OPCODE_ACTION | OPCODE_EXIT;
}

// DI instruction (0xF3)
int gb_opcode_di(__attribute__((unused)) const gb_opcode_t *opcode, gb_system_t *gb)
{
    gb->ime = 0;

    return OPCODE_ACTION;
}

// EI instruction (0xFB)
int gb_opcode_ei(__attribute__((unused)) const gb_opcode_t *opcode, gb_system_t *gb)
{
    gb->ime = 1;

    return OPCODE_ACTION;
}