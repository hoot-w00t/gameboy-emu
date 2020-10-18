/*
opcodes.h
Header file for opcodes.c
Handle opcode identification

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

#ifndef _CPU_OPCODES_H
#define _CPU_OPCODES_H

extern const opcode_t opcode_table[256];
extern const opcode_t opcode_cb_table[256];

// Return a pointer the identified opcode in opcode_table or NULL on fail
static inline const opcode_t *opcode_identify(const byte_t opcode)
{
    return opcode_table[opcode].handler ? &opcode_table[opcode] : NULL;
}

// Return a pointer the identified opcode in opcode_cb_table or NULL on fail
static inline const opcode_t *opcode_cb_identify(const byte_t opcode)
{
    return opcode_cb_table[opcode].handler ? &opcode_cb_table[opcode] : NULL;
}

#endif