/*
opcodes.h
Header file for opcodes/

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
#include <stddef.h>

#ifndef _GB_OPCODES_H
#define _GB_OPCODES_H

// int return values for the opcode handlers
#define OPCODE_ILLEGAL   (0b0000)  // Illegal opcode
#define OPCODE_ACTION    (0b0001)  // Opcode executed and action condition is true
#define OPCODE_NOACTION  (0b0010)  // Opcode executed and action condition is false
#define OPCODE_EXIT      (0b0100)  // Break out of the CPU loop
#define OPCODE_NOPC      (0b1000)  // Do not increment PC (for jump operations)

// Type definition for gb_opcode
typedef struct gb_opcode gb_opcode_t;
typedef int (*gb_opcode_handler)(const gb_opcode_t *, gb_system_t *);

struct gb_opcode {
    char *mnemonic;            // Mnemonic
    byte_t opcode;             // Opcode
    byte_t length;             // Instruction length in bytes
    byte_t cycles_true;        // Opcode duration in cycles (if conditionnal, only if condition is true)
    byte_t cycles_false;       // Opcode duration in cycles (if conditionnal and condition if false)
    char *comment;             // What the instruction does
    gb_opcode_handler handler; // Function pointer to the opcode handler
};

// Opcode function prototypes
// opcodes/control.c
int gb_opcode_nop(const gb_opcode_t *opcode, gb_system_t *gb);
int gb_opcode_stop(__attribute__((unused)) const gb_opcode_t *opcode, __attribute__((unused)) gb_system_t *gb);
int gb_opcode_di(__attribute__((unused)) const gb_opcode_t *opcode, gb_system_t *gb);
int gb_opcode_ei(__attribute__((unused)) const gb_opcode_t *opcode, gb_system_t *gb);

// opcodes/jumps.c
int gb_opcode_jp(const gb_opcode_t *opcode, gb_system_t *gb);
int gb_opcode_jr(const gb_opcode_t *opcode, gb_system_t *gb);
int gb_opcode_call(const gb_opcode_t *opcode, gb_system_t *gb);
int gb_opcode_ret(const gb_opcode_t *opcode, gb_system_t *gb);

// opcodes/ld.c
int gb_opcode_ld_rr_d16(const gb_opcode_t *opcode, gb_system_t *gb);
int gb_opcode_ld_sp_hl(const gb_opcode_t *opcode, gb_system_t *gb);
int gb_opcode_ld_hl_sp_r8(__attribute__((unused)) const gb_opcode_t *opcode, gb_system_t *gb);
int gb_opcode_ld_r_d8(const gb_opcode_t *opcode, gb_system_t *gb);
int gb_opcode_ldi(const gb_opcode_t *opcode, gb_system_t *gb);
int gb_opcode_ldd(const gb_opcode_t *opcode, gb_system_t *gb);
int gb_opcode_ldh(const gb_opcode_t *opcode, gb_system_t *gb);

// opcodes/xor.c
void gb_8b_xor(byte_t value, gb_system_t *gb);
int gb_opcode_xor(const gb_opcode_t *opcode, gb_system_t *gb);

// opcodes/or.c
void gb_8b_or(byte_t value, gb_system_t *gb);
int gb_opcode_or(const gb_opcode_t *opcode, gb_system_t *gb);

// opcodes/and.c
void gb_8b_and(byte_t value, gb_system_t *gb);
int gb_opcode_and(const gb_opcode_t *opcode, gb_system_t *gb);

// opcodes/inc.c
void gb_inc_8b_reg(byte_t reg, gb_system_t *gb);
void gb_inc_16b_reg(byte_t reg, gb_system_t *gb);
void gb_inc_address(uint16_t address, gb_system_t *gb);
int gb_opcode_inc(const gb_opcode_t *opcode, gb_system_t *gb);

// opcodes/dec.c
void gb_dec_8b_reg(byte_t reg, gb_system_t *gb);
void gb_dec_16b_reg(byte_t reg, gb_system_t *gb);
void gb_dec_address(uint16_t address, gb_system_t *gb);
int gb_opcode_dec(const gb_opcode_t *opcode, gb_system_t *gb);

// opcodes/cp.c
void gb_cp(byte_t value, gb_system_t *gb);
int gb_opcode_cp(const gb_opcode_t *opcode, gb_system_t *gb);

#endif