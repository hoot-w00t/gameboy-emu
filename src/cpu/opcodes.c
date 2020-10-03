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
#include "cpu/opcodes/ld.h"

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
    {
        .mnemonic     = "POP BC",
        .opcode       = 0xC1,
        .length       = 1,
        .cycles_true  = 12,
        .cycles_false = 12,
        .comment      = "Pop SP to BC",
        .handler      = &opcode_pop
    },
    {
        .mnemonic     = "PUSH BC",
        .opcode       = 0xC5,
        .length       = 1,
        .cycles_true  = 16,
        .cycles_false = 16,
        .comment      = "Push BC to stack",
        .handler      = &opcode_push
    },
    {
        .mnemonic     = "POP DE",
        .opcode       = 0xD1,
        .length       = 1,
        .cycles_true  = 12,
        .cycles_false = 12,
        .comment      = "Pop SP to DE",
        .handler      = &opcode_pop
    },
    {
        .mnemonic     = "PUSH DE",
        .opcode       = 0xD5,
        .length       = 1,
        .cycles_true  = 16,
        .cycles_false = 16,
        .comment      = "Push DE to stack",
        .handler      = &opcode_push
    },
    {
        .mnemonic     = "POP HL",
        .opcode       = 0xE1,
        .length       = 1,
        .cycles_true  = 12,
        .cycles_false = 12,
        .comment      = "Pop SP to HL",
        .handler      = &opcode_pop
    },
    {
        .mnemonic     = "Push HL",
        .opcode       = 0xE5,
        .length       = 1,
        .cycles_true  = 16,
        .cycles_false = 16,
        .comment      = "Push HL to stack",
        .handler      = &opcode_push
    },
    {
        .mnemonic     = "POP AF",
        .opcode       = 0xF1,
        .length       = 1,
        .cycles_true  = 12,
        .cycles_false = 12,
        .comment      = "Pop SP to AF",
        .handler      = &opcode_pop
    },
    {
        .mnemonic     = "PUSH AF",
        .opcode       = 0xF5,
        .length       = 1,
        .cycles_true  = 16,
        .cycles_false = 16,
        .comment      = "Push AF to stack",
        .handler      = &opcode_push
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