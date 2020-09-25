/*
opcodes.c
Handle opcode identification
Define opcode_table containing all the CPU's opcodes

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
#include "cpu/opcodes/control.h"

const opcode_t opcode_table[] = {
    {
        .mnemonic     = "NOP",
        .opcode       = 0x00,
        .length       = 1,
        .cycles_true  = 4,
        .cycles_false = 4,
        .comment      = "No OPeration",
        .handler      = &opcode_nop
    },
    {NULL, 0, 0, 0, 0, NULL, NULL}
};

const opcode_t *opcode_identify(byte_t opcode)
{
    for (int i = 0; opcode_table[i].handler; ++i) {
        if (opcode_table[i].opcode == opcode)
            return &opcode_table[i];
    }

    return NULL;
}