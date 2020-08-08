/*
nop.c
NOP opcode

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
#include "gb/opcodes.h"
#include "gb/memory.h"
#include "gb/registers.h"
#include "logger.h"

// Opcode table
const gb_opcode_t gb_opcode_table[] = {
    {
        .mnemonic     = "NOP",
        .opcode       = 0x00,
        .length       = 1,
        .cycles_true  = 4,
        .cycles_false = 4,
        .comment      = "No OPeration",
        .handler      = &gb_opcode_nop
    },
    {
        .mnemonic     = "LD BC,d16",
        .opcode       = 0x01,
        .length       = 3,
        .cycles_true  = 12,
        .cycles_false = 12,
        .comment      = "Load 16 bit value into register BC",
        .handler      = &gb_opcode_ld_rr_d16
    },
    {
        .mnemonic     = "LD (BC),A",
        .opcode       = 0x02,
        .length       = 1,
        .cycles_true  = 8,
        .cycles_false = 8,
        .comment      = "Load register A at address stored in register BC",
        .handler      = &gb_opcode_ld_r_r
    },
    {
        .mnemonic     = "INC BC",
        .opcode       = 0x03,
        .length       = 1,
        .cycles_true  = 8,
        .cycles_false = 8,
        .comment      = "Increment BC",
        .handler      = &gb_opcode_inc
    },
    {
        .mnemonic     = "INC B",
        .opcode       = 0x04,
        .length       = 1,
        .cycles_true  = 4,
        .cycles_false = 4,
        .comment      = "Increment B",
        .handler      = &gb_opcode_inc
    },
    {
        .mnemonic     = "DEC B",
        .opcode       = 0x05,
        .length       = 1,
        .cycles_true  = 4,
        .cycles_false = 4,
        .comment      = "Decrement B",
        .handler      = &gb_opcode_dec
    },
    {
        .mnemonic     = "LD B,d8",
        .opcode       = 0x06,
        .length       = 2,
        .cycles_true  = 8,
        .cycles_false = 8,
        .comment      = "Load 8 bit value into register B",
        .handler      = &gb_opcode_ld_r_d8
    },
    {
        .mnemonic     = "LD A,(BC)",
        .opcode       = 0x0A,
        .length       = 1,
        .cycles_true  = 8,
        .cycles_false = 8,
        .comment      = "Load value at address stored in register BC in register A",
        .handler      = &gb_opcode_ld_r_r
    },
    {
        .mnemonic     = "DEC BC",
        .opcode       = 0x0B,
        .length       = 1,
        .cycles_true  = 8,
        .cycles_false = 8,
        .comment      = "Decrement BC",
        .handler      = &gb_opcode_dec
    },
    {
        .mnemonic     = "INC C",
        .opcode       = 0x0C,
        .length       = 1,
        .cycles_true  = 4,
        .cycles_false = 4,
        .comment      = "Increment C",
        .handler      = &gb_opcode_inc
    },
    {
        .mnemonic     = "DEC C",
        .opcode       = 0x0D,
        .length       = 1,
        .cycles_true  = 4,
        .cycles_false = 4,
        .comment      = "Decrement C",
        .handler      = &gb_opcode_dec
    },
    {
        .mnemonic     = "LD C,d8",
        .opcode       = 0x0E,
        .length       = 2,
        .cycles_true  = 8,
        .cycles_false = 8,
        .comment      = "Load 8 bit value into register C",
        .handler      = &gb_opcode_ld_r_d8
    },
    {
        .mnemonic     = "STOP",
        .opcode       = 0x10,
        .length       = 2,
        .cycles_true  = 0,
        .cycles_false = 0,
        .comment      = "Low Power Standby Mode",
        .handler      = &gb_opcode_stop
    },
    {
        .mnemonic     = "LD DE,d16",
        .opcode       = 0x11,
        .length       = 3,
        .cycles_true  = 12,
        .cycles_false = 12,
        .comment      = "Load 16 bit value into register DE",
        .handler      = &gb_opcode_ld_rr_d16
    },
    {
        .mnemonic     = "LD (DE),A",
        .opcode       = 0x12,
        .length       = 1,
        .cycles_true  = 8,
        .cycles_false = 8,
        .comment      = "Load register A at address stored in register DE",
        .handler      = &gb_opcode_ld_r_r
    },
    {
        .mnemonic     = "INC DE",
        .opcode       = 0x13,
        .length       = 1,
        .cycles_true  = 8,
        .cycles_false = 8,
        .comment      = "Increment DE",
        .handler      = &gb_opcode_inc
    },
    {
        .mnemonic     = "INC D",
        .opcode       = 0x14,
        .length       = 1,
        .cycles_true  = 4,
        .cycles_false = 4,
        .comment      = "Increment D",
        .handler      = &gb_opcode_inc
    },
    {
        .mnemonic     = "DEC D",
        .opcode       = 0x15,
        .length       = 1,
        .cycles_true  = 4,
        .cycles_false = 4,
        .comment      = "Decrement D",
        .handler      = &gb_opcode_dec
    },
    {
        .mnemonic     = "LD D,d8",
        .opcode       = 0x16,
        .length       = 2,
        .cycles_true  = 8,
        .cycles_false = 8,
        .comment      = "Load 8 bit value into register D",
        .handler      = &gb_opcode_ld_r_d8
    },
    {
        .mnemonic     = "JR r8",
        .opcode       = 0x18,
        .length       = 2,
        .cycles_true  = 12,
        .cycles_false = 12,
        .comment      = "Relative jump",
        .handler      = &gb_opcode_jr
    },
    {
        .mnemonic     = "LD A,(DE)",
        .opcode       = 0x1A,
        .length       = 1,
        .cycles_true  = 8,
        .cycles_false = 8,
        .comment      = "Load value at address stored in register DE in register A",
        .handler      = &gb_opcode_ld_r_r
    },
    {
        .mnemonic     = "DEC DE",
        .opcode       = 0x1B,
        .length       = 1,
        .cycles_true  = 8,
        .cycles_false = 8,
        .comment      = "Decrement DE",
        .handler      = &gb_opcode_dec
    },
    {
        .mnemonic     = "INC E",
        .opcode       = 0x1C,
        .length       = 1,
        .cycles_true  = 4,
        .cycles_false = 4,
        .comment      = "Increment E",
        .handler      = &gb_opcode_inc
    },
    {
        .mnemonic     = "DEC E",
        .opcode       = 0x1D,
        .length       = 1,
        .cycles_true  = 4,
        .cycles_false = 4,
        .comment      = "Decrement E",
        .handler      = &gb_opcode_dec
    },
    {
        .mnemonic     = "LD E,d8",
        .opcode       = 0x1E,
        .length       = 2,
        .cycles_true  = 8,
        .cycles_false = 8,
        .comment      = "Load 8 bit value into register E",
        .handler      = &gb_opcode_ld_r_d8
    },
    {
        .mnemonic     = "JR NZ,r8",
        .opcode       = 0x20,
        .length       = 2,
        .cycles_true  = 12,
        .cycles_false = 8,
        .comment      = "Relative jump if Zero Flag is clear",
        .handler      = &gb_opcode_jr
    },
    {
        .mnemonic     = "LD HL,d16",
        .opcode       = 0x21,
        .length       = 3,
        .cycles_true  = 12,
        .cycles_false = 12,
        .comment      = "Load 16 bit value into register HL",
        .handler      = &gb_opcode_ld_rr_d16
    },
    {
        .mnemonic     = "LDI (HL),A",
        .opcode       = 0x22,
        .length       = 1,
        .cycles_true  = 8,
        .cycles_false = 8,
        .comment      = "Load register A at memory address stored in HL and increment HL",
        .handler      = &gb_opcode_ldi
    },
    {
        .mnemonic     = "INC HL",
        .opcode       = 0x23,
        .length       = 1,
        .cycles_true  = 8,
        .cycles_false = 8,
        .comment      = "Increment HL",
        .handler      = &gb_opcode_inc
    },
    {
        .mnemonic     = "INC H",
        .opcode       = 0x24,
        .length       = 1,
        .cycles_true  = 4,
        .cycles_false = 4,
        .comment      = "Increment H",
        .handler      = &gb_opcode_inc
    },
    {
        .mnemonic     = "DEC H",
        .opcode       = 0x25,
        .length       = 1,
        .cycles_true  = 4,
        .cycles_false = 4,
        .comment      = "Decrement H",
        .handler      = &gb_opcode_dec
    },
    {
        .mnemonic     = "LD H,d8",
        .opcode       = 0x26,
        .length       = 2,
        .cycles_true  = 8,
        .cycles_false = 8,
        .comment      = "Load 8 bit value into register H",
        .handler      = &gb_opcode_ld_r_d8
    },
    {
        .mnemonic     = "JR Z,r8",
        .opcode       = 0x28,
        .length       = 2,
        .cycles_true  = 12,
        .cycles_false = 8,
        .comment      = "Relative jump if Zero Flag is set",
        .handler      = &gb_opcode_jr
    },
    {
        .mnemonic     = "LDI A,(HL)",
        .opcode       = 0x2A,
        .length       = 1,
        .cycles_true  = 8,
        .cycles_false = 8,
        .comment      = "Load value at memory address stored in HL into register A and increment HL",
        .handler      = &gb_opcode_ldi
    },
    {
        .mnemonic     = "DEC HL",
        .opcode       = 0x2B,
        .length       = 1,
        .cycles_true  = 8,
        .cycles_false = 8,
        .comment      = "Decrement HL",
        .handler      = &gb_opcode_dec
    },
    {
        .mnemonic     = "INC L",
        .opcode       = 0x2C,
        .length       = 1,
        .cycles_true  = 4,
        .cycles_false = 4,
        .comment      = "Increment L",
        .handler      = &gb_opcode_inc
    },
    {
        .mnemonic     = "DEC L",
        .opcode       = 0x2D,
        .length       = 1,
        .cycles_true  = 4,
        .cycles_false = 4,
        .comment      = "Decrement L",
        .handler      = &gb_opcode_dec
    },
    {
        .mnemonic     = "LD L,d8",
        .opcode       = 0x2E,
        .length       = 2,
        .cycles_true  = 8,
        .cycles_false = 8,
        .comment      = "Load 8 bit value into register L",
        .handler      = &gb_opcode_ld_r_d8
    },
    {
        .mnemonic     = "JR NC,r8",
        .opcode       = 0x30,
        .length       = 2,
        .cycles_true  = 12,
        .cycles_false = 8,
        .comment      = "Relative jump if Carry Flag is clear",
        .handler      = &gb_opcode_jr
    },
    {
        .mnemonic     = "LD SP,d16",
        .opcode       = 0x31,
        .length       = 3,
        .cycles_true  = 12,
        .cycles_false = 12,
        .comment      = "Load 16 bit value into SP",
        .handler      = &gb_opcode_ld_rr_d16
    },
    {
        .mnemonic     = "LDD (HL),A",
        .opcode       = 0x32,
        .length       = 1,
        .cycles_true  = 8,
        .cycles_false = 8,
        .comment      = "Load register A at memory address stored in HL and decrement HL",
        .handler      = &gb_opcode_ldd
    },
    {
        .mnemonic     = "INC SP",
        .opcode       = 0x33,
        .length       = 1,
        .cycles_true  = 8,
        .cycles_false = 8,
        .comment      = "Increment SP",
        .handler      = &gb_opcode_inc
    },
    {
        .mnemonic     = "INC (HL)",
        .opcode       = 0x34,
        .length       = 1,
        .cycles_true  = 12,
        .cycles_false = 12,
        .comment      = "Increment value at address stored in HL",
        .handler      = &gb_opcode_inc
    },
    {
        .mnemonic     = "DEC (HL)",
        .opcode       = 0x35,
        .length       = 1,
        .cycles_true  = 12,
        .cycles_false = 12,
        .comment      = "Decrement value at address stored in HL",
        .handler      = &gb_opcode_dec
    },
    {
        .mnemonic     = "LD (HL),d8",
        .opcode       = 0x36,
        .length       = 2,
        .cycles_true  = 12,
        .cycles_false = 12,
        .comment      = "Load 8 bit value at address stored in register HL",
        .handler      = &gb_opcode_ld_r_d8
    },
    {
        .mnemonic     = "JR C,r8",
        .opcode       = 0x38,
        .length       = 2,
        .cycles_true  = 12,
        .cycles_false = 8,
        .comment      = "Relative jump if Carry Flag is set",
        .handler      = &gb_opcode_jr
    },
    {
        .mnemonic     = "LDD A,(HL)",
        .opcode       = 0x3A,
        .length       = 1,
        .cycles_true  = 8,
        .cycles_false = 8,
        .comment      = "Load value at memory address stored in HL into register A and decrement HL",
        .handler      = &gb_opcode_ldd
    },
    {
        .mnemonic     = "DEC SP",
        .opcode       = 0x3B,
        .length       = 1,
        .cycles_true  = 8,
        .cycles_false = 8,
        .comment      = "Decrement SP",
        .handler      = &gb_opcode_dec
    },
    {
        .mnemonic     = "INC A",
        .opcode       = 0x3C,
        .length       = 1,
        .cycles_true  = 4,
        .cycles_false = 4,
        .comment      = "Increment A",
        .handler      = &gb_opcode_inc
    },
    {
        .mnemonic     = "DEC A",
        .opcode       = 0x3D,
        .length       = 1,
        .cycles_true  = 4,
        .cycles_false = 4,
        .comment      = "Decrement A",
        .handler      = &gb_opcode_dec
    },
    {
        .mnemonic     = "LD A,d8",
        .opcode       = 0x3E,
        .length       = 2,
        .cycles_true  = 8,
        .cycles_false = 8,
        .comment      = "Load 8 bit value into register A",
        .handler      = &gb_opcode_ld_r_d8
    },
    {
        .mnemonic     = "LD B,B",
        .opcode       = 0x40,
        .length       = 1,
        .cycles_true  = 4,
        .cycles_false = 4,
        .comment      = "Load register B into register B",
        .handler      = &gb_opcode_ld_r_r
    },
    {
        .mnemonic     = "LD B,C",
        .opcode       = 0x41,
        .length       = 1,
        .cycles_true  = 4,
        .cycles_false = 4,
        .comment      = "Load register C into register B",
        .handler      = &gb_opcode_ld_r_r
    },
    {
        .mnemonic     = "LD B,D",
        .opcode       = 0x42,
        .length       = 1,
        .cycles_true  = 4,
        .cycles_false = 4,
        .comment      = "Load register D into register B",
        .handler      = &gb_opcode_ld_r_r
    },
    {
        .mnemonic     = "LD B,E",
        .opcode       = 0x43,
        .length       = 1,
        .cycles_true  = 4,
        .cycles_false = 4,
        .comment      = "Load register E into register B",
        .handler      = &gb_opcode_ld_r_r
    },
    {
        .mnemonic     = "LD B,H",
        .opcode       = 0x44,
        .length       = 1,
        .cycles_true  = 4,
        .cycles_false = 4,
        .comment      = "Load register H into register B",
        .handler      = &gb_opcode_ld_r_r
    },
    {
        .mnemonic     = "LD B,L",
        .opcode       = 0x45,
        .length       = 1,
        .cycles_true  = 4,
        .cycles_false = 4,
        .comment      = "Load register L into register B",
        .handler      = &gb_opcode_ld_r_r
    },
    {
        .mnemonic     = "LD B,(HL)",
        .opcode       = 0x46,
        .length       = 1,
        .cycles_true  = 8,
        .cycles_false = 8,
        .comment      = "Load value at address stored in register HL into register B",
        .handler      = &gb_opcode_ld_r_r
    },
    {
        .mnemonic     = "LD B,A",
        .opcode       = 0x47,
        .length       = 1,
        .cycles_true  = 4,
        .cycles_false = 4,
        .comment      = "Load register A into register B",
        .handler      = &gb_opcode_ld_r_r
    },
    {
        .mnemonic     = "LD C,B",
        .opcode       = 0x48,
        .length       = 1,
        .cycles_true  = 4,
        .cycles_false = 4,
        .comment      = "Load register B into register C",
        .handler      = &gb_opcode_ld_r_r
    },
    {
        .mnemonic     = "LD C,C",
        .opcode       = 0x49,
        .length       = 1,
        .cycles_true  = 4,
        .cycles_false = 4,
        .comment      = "Load register C into register C",
        .handler      = &gb_opcode_ld_r_r
    },
    {
        .mnemonic     = "LD C,D",
        .opcode       = 0x4A,
        .length       = 1,
        .cycles_true  = 4,
        .cycles_false = 4,
        .comment      = "Load register D into register C",
        .handler      = &gb_opcode_ld_r_r
    },
    {
        .mnemonic     = "LD C,E",
        .opcode       = 0x4B,
        .length       = 1,
        .cycles_true  = 4,
        .cycles_false = 4,
        .comment      = "Load register E into register C",
        .handler      = &gb_opcode_ld_r_r
    },
    {
        .mnemonic     = "LD C,H",
        .opcode       = 0x4C,
        .length       = 1,
        .cycles_true  = 4,
        .cycles_false = 4,
        .comment      = "Load register H into register C",
        .handler      = &gb_opcode_ld_r_r
    },
    {
        .mnemonic     = "LD C,L",
        .opcode       = 0x4D,
        .length       = 1,
        .cycles_true  = 4,
        .cycles_false = 4,
        .comment      = "Load register L into register C",
        .handler      = &gb_opcode_ld_r_r
    },
    {
        .mnemonic     = "LD C,(HL)",
        .opcode       = 0x4E,
        .length       = 1,
        .cycles_true  = 8,
        .cycles_false = 8,
        .comment      = "Load value at address stored in register HL into register C",
        .handler      = &gb_opcode_ld_r_r
    },
    {
        .mnemonic     = "LD C,A",
        .opcode       = 0x4F,
        .length       = 1,
        .cycles_true  = 4,
        .cycles_false = 4,
        .comment      = "Load register A into register C",
        .handler      = &gb_opcode_ld_r_r
    },
    {
        .mnemonic     = "LD D,B",
        .opcode       = 0x50,
        .length       = 1,
        .cycles_true  = 4,
        .cycles_false = 4,
        .comment      = "Load register B into register D",
        .handler      = &gb_opcode_ld_r_r
    },
    {
        .mnemonic     = "LD D,C",
        .opcode       = 0x51,
        .length       = 1,
        .cycles_true  = 4,
        .cycles_false = 4,
        .comment      = "Load register C into register D",
        .handler      = &gb_opcode_ld_r_r
    },
    {
        .mnemonic     = "LD D,D",
        .opcode       = 0x52,
        .length       = 1,
        .cycles_true  = 4,
        .cycles_false = 4,
        .comment      = "Load register D into register D",
        .handler      = &gb_opcode_ld_r_r
    },
    {
        .mnemonic     = "LD D,E",
        .opcode       = 0x53,
        .length       = 1,
        .cycles_true  = 4,
        .cycles_false = 4,
        .comment      = "Load register E into register D",
        .handler      = &gb_opcode_ld_r_r
    },
    {
        .mnemonic     = "LD D,H",
        .opcode       = 0x54,
        .length       = 1,
        .cycles_true  = 4,
        .cycles_false = 4,
        .comment      = "Load register H into register D",
        .handler      = &gb_opcode_ld_r_r
    },
    {
        .mnemonic     = "LD D,L",
        .opcode       = 0x55,
        .length       = 1,
        .cycles_true  = 4,
        .cycles_false = 4,
        .comment      = "Load register L into register D",
        .handler      = &gb_opcode_ld_r_r
    },
    {
        .mnemonic     = "LD D,(HL)",
        .opcode       = 0x56,
        .length       = 1,
        .cycles_true  = 8,
        .cycles_false = 8,
        .comment      = "Load value at address stored in register HL into register D",
        .handler      = &gb_opcode_ld_r_r
    },
    {
        .mnemonic     = "LD D,A",
        .opcode       = 0x57,
        .length       = 1,
        .cycles_true  = 4,
        .cycles_false = 4,
        .comment      = "Load register A into register D",
        .handler      = &gb_opcode_ld_r_r
    },
    {
        .mnemonic     = "LD E,B",
        .opcode       = 0x58,
        .length       = 1,
        .cycles_true  = 4,
        .cycles_false = 4,
        .comment      = "Load register B into register E",
        .handler      = &gb_opcode_ld_r_r
    },
    {
        .mnemonic     = "LD E,C",
        .opcode       = 0x59,
        .length       = 1,
        .cycles_true  = 4,
        .cycles_false = 4,
        .comment      = "Load register C into register E",
        .handler      = &gb_opcode_ld_r_r
    },
    {
        .mnemonic     = "LD E,D",
        .opcode       = 0x5A,
        .length       = 1,
        .cycles_true  = 4,
        .cycles_false = 4,
        .comment      = "Load register D into register E",
        .handler      = &gb_opcode_ld_r_r
    },
    {
        .mnemonic     = "LD E,E",
        .opcode       = 0x5B,
        .length       = 1,
        .cycles_true  = 4,
        .cycles_false = 4,
        .comment      = "Load register E into register E",
        .handler      = &gb_opcode_ld_r_r
    },
    {
        .mnemonic     = "LD E,H",
        .opcode       = 0x5C,
        .length       = 1,
        .cycles_true  = 4,
        .cycles_false = 4,
        .comment      = "Load register H into register E",
        .handler      = &gb_opcode_ld_r_r
    },
    {
        .mnemonic     = "LD E,L",
        .opcode       = 0x5D,
        .length       = 1,
        .cycles_true  = 4,
        .cycles_false = 4,
        .comment      = "Load register L into register E",
        .handler      = &gb_opcode_ld_r_r
    },
    {
        .mnemonic     = "LD E,(HL)",
        .opcode       = 0x5E,
        .length       = 1,
        .cycles_true  = 8,
        .cycles_false = 8,
        .comment      = "Load value at address stored in register HL into register E",
        .handler      = &gb_opcode_ld_r_r
    },
    {
        .mnemonic     = "LD E,A",
        .opcode       = 0x5F,
        .length       = 1,
        .cycles_true  = 4,
        .cycles_false = 4,
        .comment      = "Load register A into register E",
        .handler      = &gb_opcode_ld_r_r
    },
    {
        .mnemonic     = "LD H,B",
        .opcode       = 0x60,
        .length       = 1,
        .cycles_true  = 4,
        .cycles_false = 4,
        .comment      = "Load register B into register H",
        .handler      = &gb_opcode_ld_r_r
    },
    {
        .mnemonic     = "LD H,C",
        .opcode       = 0x61,
        .length       = 1,
        .cycles_true  = 4,
        .cycles_false = 4,
        .comment      = "Load register C into register H",
        .handler      = &gb_opcode_ld_r_r
    },
    {
        .mnemonic     = "LD H,D",
        .opcode       = 0x62,
        .length       = 1,
        .cycles_true  = 4,
        .cycles_false = 4,
        .comment      = "Load register D into register H",
        .handler      = &gb_opcode_ld_r_r
    },
    {
        .mnemonic     = "LD H,E",
        .opcode       = 0x63,
        .length       = 1,
        .cycles_true  = 4,
        .cycles_false = 4,
        .comment      = "Load register E into register H",
        .handler      = &gb_opcode_ld_r_r
    },
    {
        .mnemonic     = "LD H,H",
        .opcode       = 0x64,
        .length       = 1,
        .cycles_true  = 4,
        .cycles_false = 4,
        .comment      = "Load register H into register H",
        .handler      = &gb_opcode_ld_r_r
    },
    {
        .mnemonic     = "LD H,L",
        .opcode       = 0x65,
        .length       = 1,
        .cycles_true  = 4,
        .cycles_false = 4,
        .comment      = "Load register L into register H",
        .handler      = &gb_opcode_ld_r_r
    },
    {
        .mnemonic     = "LD H,(HL)",
        .opcode       = 0x66,
        .length       = 1,
        .cycles_true  = 8,
        .cycles_false = 8,
        .comment      = "Load value at address stored in register HL into register H",
        .handler      = &gb_opcode_ld_r_r
    },
    {
        .mnemonic     = "LD H,A",
        .opcode       = 0x67,
        .length       = 1,
        .cycles_true  = 4,
        .cycles_false = 4,
        .comment      = "Load register A into register H",
        .handler      = &gb_opcode_ld_r_r
    },
    {
        .mnemonic     = "LD L,B",
        .opcode       = 0x68,
        .length       = 1,
        .cycles_true  = 4,
        .cycles_false = 4,
        .comment      = "Load register B into register L",
        .handler      = &gb_opcode_ld_r_r
    },
    {
        .mnemonic     = "LD L,C",
        .opcode       = 0x69,
        .length       = 1,
        .cycles_true  = 4,
        .cycles_false = 4,
        .comment      = "Load register C into register L",
        .handler      = &gb_opcode_ld_r_r
    },
    {
        .mnemonic     = "LD L,D",
        .opcode       = 0x6A,
        .length       = 1,
        .cycles_true  = 4,
        .cycles_false = 4,
        .comment      = "Load register D into register L",
        .handler      = &gb_opcode_ld_r_r
    },
    {
        .mnemonic     = "LD L,E",
        .opcode       = 0x6B,
        .length       = 1,
        .cycles_true  = 4,
        .cycles_false = 4,
        .comment      = "Load register E into register L",
        .handler      = &gb_opcode_ld_r_r
    },
    {
        .mnemonic     = "LD L,H",
        .opcode       = 0x6C,
        .length       = 1,
        .cycles_true  = 4,
        .cycles_false = 4,
        .comment      = "Load register H into register L",
        .handler      = &gb_opcode_ld_r_r
    },
    {
        .mnemonic     = "LD L,L",
        .opcode       = 0x6D,
        .length       = 1,
        .cycles_true  = 4,
        .cycles_false = 4,
        .comment      = "Load register L into register L",
        .handler      = &gb_opcode_ld_r_r
    },
    {
        .mnemonic     = "LD L,(HL)",
        .opcode       = 0x6E,
        .length       = 1,
        .cycles_true  = 8,
        .cycles_false = 8,
        .comment      = "Load value at address stored in register HL into register L",
        .handler      = &gb_opcode_ld_r_r
    },
    {
        .mnemonic     = "LD L,A",
        .opcode       = 0x6F,
        .length       = 1,
        .cycles_true  = 4,
        .cycles_false = 4,
        .comment      = "Load register A into register L",
        .handler      = &gb_opcode_ld_r_r
    },
    {
        .mnemonic     = "LD (HL),B",
        .opcode       = 0x70,
        .length       = 1,
        .cycles_true  = 8,
        .cycles_false = 8,
        .comment      = "Load register B at address stored in register HL",
        .handler      = &gb_opcode_ld_r_r
    },
    {
        .mnemonic     = "LD (HL),C",
        .opcode       = 0x71,
        .length       = 1,
        .cycles_true  = 8,
        .cycles_false = 8,
        .comment      = "Load register C at address stored in register HL",
        .handler      = &gb_opcode_ld_r_r
    },
    {
        .mnemonic     = "LD (HL),D",
        .opcode       = 0x72,
        .length       = 1,
        .cycles_true  = 8,
        .cycles_false = 8,
        .comment      = "Load register D at address stored in register HL",
        .handler      = &gb_opcode_ld_r_r
    },
    {
        .mnemonic     = "LD (HL),E",
        .opcode       = 0x73,
        .length       = 1,
        .cycles_true  = 8,
        .cycles_false = 8,
        .comment      = "Load register E at address stored in register HL",
        .handler      = &gb_opcode_ld_r_r
    },
    {
        .mnemonic     = "LD (HL),H",
        .opcode       = 0x74,
        .length       = 1,
        .cycles_true  = 8,
        .cycles_false = 8,
        .comment      = "Load register H at address stored in register HL",
        .handler      = &gb_opcode_ld_r_r
    },
    {
        .mnemonic     = "LD (HL),L",
        .opcode       = 0x75,
        .length       = 1,
        .cycles_true  = 8,
        .cycles_false = 8,
        .comment      = "Load register L at address stored in register HL",
        .handler      = &gb_opcode_ld_r_r
    },
    {
        .mnemonic     = "LD (HL),A",
        .opcode       = 0x77,
        .length       = 1,
        .cycles_true  = 8,
        .cycles_false = 8,
        .comment      = "Load register A at address stored in register HL",
        .handler      = &gb_opcode_ld_r_r
    },
    {
        .mnemonic     = "LD A,B",
        .opcode       = 0x78,
        .length       = 1,
        .cycles_true  = 4,
        .cycles_false = 4,
        .comment      = "Load register B into register A",
        .handler      = &gb_opcode_ld_r_r
    },
    {
        .mnemonic     = "LD A,C",
        .opcode       = 0x79,
        .length       = 1,
        .cycles_true  = 4,
        .cycles_false = 4,
        .comment      = "Load register C into register A",
        .handler      = &gb_opcode_ld_r_r
    },
    {
        .mnemonic     = "LD A,D",
        .opcode       = 0x7A,
        .length       = 1,
        .cycles_true  = 4,
        .cycles_false = 4,
        .comment      = "Load register D into register A",
        .handler      = &gb_opcode_ld_r_r
    },
    {
        .mnemonic     = "LD A,E",
        .opcode       = 0x7B,
        .length       = 1,
        .cycles_true  = 4,
        .cycles_false = 4,
        .comment      = "Load register E into register A",
        .handler      = &gb_opcode_ld_r_r
    },
    {
        .mnemonic     = "LD A,H",
        .opcode       = 0x7C,
        .length       = 1,
        .cycles_true  = 4,
        .cycles_false = 4,
        .comment      = "Load register H into register A",
        .handler      = &gb_opcode_ld_r_r
    },
    {
        .mnemonic     = "LD A,L",
        .opcode       = 0x7D,
        .length       = 1,
        .cycles_true  = 4,
        .cycles_false = 4,
        .comment      = "Load register L into register A",
        .handler      = &gb_opcode_ld_r_r
    },
    {
        .mnemonic     = "LD A,(HL)",
        .opcode       = 0x7E,
        .length       = 1,
        .cycles_true  = 8,
        .cycles_false = 8,
        .comment      = "Load value at address stored in register HL into register A",
        .handler      = &gb_opcode_ld_r_r
    },
    {
        .mnemonic     = "LD A,A",
        .opcode       = 0x7F,
        .length       = 1,
        .cycles_true  = 4,
        .cycles_false = 4,
        .comment      = "Load register A into register A",
        .handler      = &gb_opcode_ld_r_r
    },
    {
        .mnemonic     = "AND B",
        .opcode       = 0xA0,
        .length       = 1,
        .cycles_true  = 4,
        .cycles_false = 4,
        .comment      = "And between registers A and B",
        .handler      = &gb_opcode_and
    },
    {
        .mnemonic     = "AND C",
        .opcode       = 0xA1,
        .length       = 1,
        .cycles_true  = 4,
        .cycles_false = 4,
        .comment      = "And between registers A and C",
        .handler      = &gb_opcode_and
    },
    {
        .mnemonic     = "AND D",
        .opcode       = 0xA2,
        .length       = 1,
        .cycles_true  = 4,
        .cycles_false = 4,
        .comment      = "And between registers A and D",
        .handler      = &gb_opcode_and
    },
    {
        .mnemonic     = "AND E",
        .opcode       = 0xA3,
        .length       = 1,
        .cycles_true  = 4,
        .cycles_false = 4,
        .comment      = "And between registers A and E",
        .handler      = &gb_opcode_and
    },
    {
        .mnemonic     = "AND H",
        .opcode       = 0xA4,
        .length       = 1,
        .cycles_true  = 4,
        .cycles_false = 4,
        .comment      = "And between registers A and H",
        .handler      = &gb_opcode_and
    },
    {
        .mnemonic     = "AND L",
        .opcode       = 0xA5,
        .length       = 1,
        .cycles_true  = 4,
        .cycles_false = 4,
        .comment      = "And between registers A and L",
        .handler      = &gb_opcode_and
    },
    {
        .mnemonic     = "AND (HL)",
        .opcode       = 0xA6,
        .length       = 1,
        .cycles_true  = 8,
        .cycles_false = 8,
        .comment      = "And between register A and value at address stored in HL",
        .handler      = &gb_opcode_and
    },
    {
        .mnemonic     = "AND A",
        .opcode       = 0xA7,
        .length       = 1,
        .cycles_true  = 4,
        .cycles_false = 4,
        .comment      = "And between registers A and A",
        .handler      = &gb_opcode_and
    },
    {
        .mnemonic     = "XOR B",
        .opcode       = 0xA8,
        .length       = 1,
        .cycles_true  = 4,
        .cycles_false = 4,
        .comment      = "Exclusive OR between registers A and B",
        .handler      = &gb_opcode_xor
    },
    {
        .mnemonic     = "XOR C",
        .opcode       = 0xA9,
        .length       = 1,
        .cycles_true  = 4,
        .cycles_false = 4,
        .comment      = "Exclusive OR between registers A and C",
        .handler      = &gb_opcode_xor
    },
    {
        .mnemonic     = "XOR D",
        .opcode       = 0xAA,
        .length       = 1,
        .cycles_true  = 4,
        .cycles_false = 4,
        .comment      = "Exclusive OR between registers A and D",
        .handler      = &gb_opcode_xor
    },
    {
        .mnemonic     = "XOR E",
        .opcode       = 0xAB,
        .length       = 1,
        .cycles_true  = 4,
        .cycles_false = 4,
        .comment      = "Exclusive OR between registers A and E",
        .handler      = &gb_opcode_xor
    },
    {
        .mnemonic     = "XOR H",
        .opcode       = 0xAC,
        .length       = 1,
        .cycles_true  = 4,
        .cycles_false = 4,
        .comment      = "Exclusive OR between registers A and H",
        .handler      = &gb_opcode_xor
    },
    {
        .mnemonic     = "XOR L",
        .opcode       = 0xAD,
        .length       = 1,
        .cycles_true  = 4,
        .cycles_false = 4,
        .comment      = "Exclusive OR between registers A and L",
        .handler      = &gb_opcode_xor
    },
    {
        .mnemonic     = "XOR (HL)",
        .opcode       = 0xAE,
        .length       = 1,
        .cycles_true  = 8,
        .cycles_false = 4,
        .comment      = "Exclusive OR between register A and value at address stored in HL",
        .handler      = &gb_opcode_xor
    },
    {
        .mnemonic     = "XOR A",
        .opcode       = 0xAF,
        .length       = 1,
        .cycles_true  = 4,
        .cycles_false = 4,
        .comment      = "Exclusive OR between registers A and A",
        .handler      = &gb_opcode_xor
    },
    {
        .mnemonic     = "OR B",
        .opcode       = 0xB0,
        .length       = 1,
        .cycles_true  = 4,
        .cycles_false = 4,
        .comment      = "OR between registers A and B",
        .handler      = &gb_opcode_or
    },
    {
        .mnemonic     = "OR C",
        .opcode       = 0xB1,
        .length       = 1,
        .cycles_true  = 4,
        .cycles_false = 4,
        .comment      = "OR between registers A and C",
        .handler      = &gb_opcode_or
    },
    {
        .mnemonic     = "OR D",
        .opcode       = 0xB2,
        .length       = 1,
        .cycles_true  = 4,
        .cycles_false = 4,
        .comment      = "OR between registers A and D",
        .handler      = &gb_opcode_or
    },
    {
        .mnemonic     = "OR E",
        .opcode       = 0xB3,
        .length       = 1,
        .cycles_true  = 4,
        .cycles_false = 4,
        .comment      = "OR between registers A and E",
        .handler      = &gb_opcode_or
    },
    {
        .mnemonic     = "OR H",
        .opcode       = 0xB4,
        .length       = 1,
        .cycles_true  = 4,
        .cycles_false = 4,
        .comment      = "OR between registers A and H",
        .handler      = &gb_opcode_or
    },
    {
        .mnemonic     = "OR L",
        .opcode       = 0xB5,
        .length       = 1,
        .cycles_true  = 4,
        .cycles_false = 4,
        .comment      = "OR between registers A and L",
        .handler      = &gb_opcode_or
    },
    {
        .mnemonic     = "OR (HL)",
        .opcode       = 0xB6,
        .length       = 1,
        .cycles_true  = 8,
        .cycles_false = 8,
        .comment      = "OR between registers A and value at address stored in HL",
        .handler      = &gb_opcode_or
    },
    {
        .mnemonic     = "OR A",
        .opcode       = 0xB7,
        .length       = 1,
        .cycles_true  = 4,
        .cycles_false = 4,
        .comment      = "OR between registers A and A",
        .handler      = &gb_opcode_or
    },
    {
        .mnemonic     = "CP B",
        .opcode       = 0xB8,
        .length       = 1,
        .cycles_true  = 4,
        .cycles_false = 4,
        .comment      = "CP registers A and B",
        .handler      = &gb_opcode_cp
    },
    {
        .mnemonic     = "CP C",
        .opcode       = 0xB9,
        .length       = 1,
        .cycles_true  = 4,
        .cycles_false = 4,
        .comment      = "CP registers A and C",
        .handler      = &gb_opcode_cp
    },
    {
        .mnemonic     = "CP D",
        .opcode       = 0xBA,
        .length       = 1,
        .cycles_true  = 4,
        .cycles_false = 4,
        .comment      = "CP registers A and D",
        .handler      = &gb_opcode_cp
    },
    {
        .mnemonic     = "CP E",
        .opcode       = 0xBB,
        .length       = 1,
        .cycles_true  = 4,
        .cycles_false = 4,
        .comment      = "CP registers A and E",
        .handler      = &gb_opcode_cp
    },
    {
        .mnemonic     = "CP H",
        .opcode       = 0xBC,
        .length       = 1,
        .cycles_true  = 4,
        .cycles_false = 4,
        .comment      = "CP registers A and H",
        .handler      = &gb_opcode_cp
    },
    {
        .mnemonic     = "CP L",
        .opcode       = 0xBD,
        .length       = 1,
        .cycles_true  = 4,
        .cycles_false = 4,
        .comment      = "CP registers A and L",
        .handler      = &gb_opcode_cp
    },
    {
        .mnemonic     = "CP (HL)",
        .opcode       = 0xBE,
        .length       = 1,
        .cycles_true  = 8,
        .cycles_false = 8,
        .comment      = "CP register A and value at address stored in HL",
        .handler      = &gb_opcode_cp
    },
    {
        .mnemonic     = "CP A",
        .opcode       = 0xBF,
        .length       = 1,
        .cycles_true  = 4,
        .cycles_false = 4,
        .comment      = "CP registers A and A",
        .handler      = &gb_opcode_cp
    },
    {
        .mnemonic     = "RET NZ",
        .opcode       = 0xC0,
        .length       = 1,
        .cycles_true  = 20,
        .cycles_false = 8,
        .comment      = "Return if Zero Flag is clear",
        .handler      = &gb_opcode_ret
    },
    {
        .mnemonic     = "JP NZ,a16",
        .opcode       = 0xC2,
        .length       = 3,
        .cycles_true  = 16,
        .cycles_false = 12,
        .comment      = "Jump to 16 bit address if Zero Flag is clear",
        .handler      = &gb_opcode_jp
    },
    {
        .mnemonic     = "JP a16",
        .opcode       = 0xC3,
        .length       = 3,
        .cycles_true  = 16,
        .cycles_false = 16,
        .comment      = "Jump to 16 bit address",
        .handler      = &gb_opcode_jp
    },
    {
        .mnemonic     = "CALL NZ,a16",
        .opcode       = 0xC4,
        .length       = 3,
        .cycles_true  = 24,
        .cycles_false = 12,
        .comment      = "Call 16 bit address if Zero Flag is clear",
        .handler      = &gb_opcode_call
    },
    {
        .mnemonic     = "RET Z",
        .opcode       = 0xC8,
        .length       = 1,
        .cycles_true  = 20,
        .cycles_false = 8,
        .comment      = "Return if Zero Flag is set",
        .handler      = &gb_opcode_ret
    },
    {
        .mnemonic     = "RET",
        .opcode       = 0xC9,
        .length       = 1,
        .cycles_true  = 16,
        .cycles_false = 16,
        .comment      = "Return",
        .handler      = &gb_opcode_ret
    },
    {
        .mnemonic     = "JP Z,a16",
        .opcode       = 0xCA,
        .length       = 3,
        .cycles_true  = 16,
        .cycles_false = 12,
        .comment      = "Jump to 16 bit address if Zero Flag is set",
        .handler      = &gb_opcode_jp
    },
    {
        .mnemonic     = "CALL Z,a16",
        .opcode       = 0xCC,
        .length       = 3,
        .cycles_true  = 24,
        .cycles_false = 12,
        .comment      = "Call 16 bit address if Zero Flag is set",
        .handler      = &gb_opcode_call
    },
    {
        .mnemonic     = "CALL a16",
        .opcode       = 0xCD,
        .length       = 3,
        .cycles_true  = 12,
        .cycles_false = 12,
        .comment      = "Call 16 bit address",
        .handler      = &gb_opcode_call
    },
    {
        .mnemonic     = "RET NC",
        .opcode       = 0xD0,
        .length       = 1,
        .cycles_true  = 20,
        .cycles_false = 8,
        .comment      = "Return if Carry Flag is clear",
        .handler      = &gb_opcode_ret
    },
    {
        .mnemonic     = "JP NC,a16",
        .opcode       = 0xD2,
        .length       = 3,
        .cycles_true  = 16,
        .cycles_false = 12,
        .comment      = "Jump to 16 bit address if Carry Flag is clear",
        .handler      = &gb_opcode_jp
    },
    {
        .mnemonic     = "CALL NC,a16",
        .opcode       = 0xD4,
        .length       = 3,
        .cycles_true  = 24,
        .cycles_false = 12,
        .comment      = "Call 16 bit address if Carry Flag is clear",
        .handler      = &gb_opcode_call
    },
    {
        .mnemonic     = "RET C",
        .opcode       = 0xD8,
        .length       = 1,
        .cycles_true  = 20,
        .cycles_false = 8,
        .comment      = "Return if Carry Flag is set",
        .handler      = &gb_opcode_ret
    },
    {
        .mnemonic     = "RETI",
        .opcode       = 0xD9,
        .length       = 1,
        .cycles_true  = 16,
        .cycles_false = 16,
        .comment      = "Return and enable interrupts",
        .handler      = &gb_opcode_ret
    },
    {
        .mnemonic     = "JP C,a16",
        .opcode       = 0xDA,
        .length       = 3,
        .cycles_true  = 16,
        .cycles_false = 12,
        .comment      = "Jump to 16 bit address if Carry Flag is set",
        .handler      = &gb_opcode_jp
    },
    {
        .mnemonic     = "CALL C,a16",
        .opcode       = 0xDC,
        .length       = 3,
        .cycles_true  = 24,
        .cycles_false = 12,
        .comment      = "Call 16 bit address if Carry Flag is set",
        .handler      = &gb_opcode_call
    },
    {
        .mnemonic     = "LDH d8,A",
        .opcode       = 0xE0,
        .length       = 2,
        .cycles_true  = 12,
        .cycles_false = 12,
        .comment      = "Load register A into memory address 0xFF00+d8",
        .handler      = &gb_opcode_ldh
    },
    {
        .mnemonic     = "AND d8",
        .opcode       = 0xE6,
        .length       = 2,
        .cycles_true  = 8,
        .cycles_false = 8,
        .comment      = "And between register A and 8 bit value",
        .handler      = &gb_opcode_and
    },
    {
        .mnemonic     = "JP (HL)",
        .opcode       = 0xE9,
        .length       = 1,
        .cycles_true  = 4,
        .cycles_false = 4,
        .comment      = "Jump to address stored in HL register",
        .handler      = &gb_opcode_jp
    },
    {
        .mnemonic     = "LD (a16),A",
        .opcode       = 0xEA,
        .length       = 3,
        .cycles_true  = 16,
        .cycles_false = 16,
        .comment      = "Load register A at address",
        .handler      = &gb_opcode_ld_r_r
    },
    {
        .mnemonic     = "XOR d8",
        .opcode       = 0xEE,
        .length       = 2,
        .cycles_true  = 8,
        .cycles_false = 8,
        .comment      = "Exclusive OR between register A and d8",
        .handler      = &gb_opcode_xor
    },
    {
        .mnemonic     = "LDH A,d8",
        .opcode       = 0xF0,
        .length       = 2,
        .cycles_true  = 12,
        .cycles_false = 12,
        .comment      = "Load memory address 0xFF00+d8 into register",
        .handler      = &gb_opcode_ldh
    },
    {
        .mnemonic     = "DI",
        .opcode       = 0xF3,
        .length       = 1,
        .cycles_true  = 4,
        .cycles_false = 4,
        .comment      = "Disable Interrupts",
        .handler      = &gb_opcode_di
    },
    {
        .mnemonic     = "OR d8",
        .opcode       = 0xF6,
        .length       = 2,
        .cycles_true  = 8,
        .cycles_false = 8,
        .comment      = "OR between register A and 8 bit value",
        .handler      = &gb_opcode_or
    },
    {
        .mnemonic     = "LD HL,SP+r8",
        .opcode       = 0xF8,
        .length       = 2,
        .cycles_true  = 12,
        .cycles_false = 12,
        .comment      = "Load 16 bit register HL into SP",
        .handler      = &gb_opcode_ld_hl_sp_r8
    },
    {
        .mnemonic     = "LD SP,HL",
        .opcode       = 0xF9,
        .length       = 1,
        .cycles_true  = 8,
        .cycles_false = 8,
        .comment      = "Load 16 bit register HL into SP",
        .handler      = &gb_opcode_ld_sp_hl
    },
    {
        .mnemonic     = "LD A,(a16)",
        .opcode       = 0xFA,
        .length       = 3,
        .cycles_true  = 16,
        .cycles_false = 16,
        .comment      = "Load value at address into register A",
        .handler      = &gb_opcode_ld_r_r
    },
    {
        .mnemonic     = "EI",
        .opcode       = 0xFB,
        .length       = 1,
        .cycles_true  = 4,
        .cycles_false = 4,
        .comment      = "Enable Interrupts",
        .handler      = &gb_opcode_ei
    },
    {
        .mnemonic     = "CP d8",
        .opcode       = 0xFE,
        .length       = 2,
        .cycles_true  = 8,
        .cycles_false = 8,
        .comment      = "CP register A with 8 bit value",
        .handler      = &gb_opcode_cp
    },
    {NULL, 0, 0, 0, 0, NULL, NULL}
};

// Push address to stack
void gb_cpu_push_stack(const uint16_t address, gb_system_t *gb)
{
    gb_write_byte(gb->sp, (address >> 8) & 0xff, false, gb);
    gb_write_byte(gb->sp - 1, address & 0xff, false, gb);

    gb->sp -= 2;
}

// Pop address from stack
uint16_t gb_cpu_pop_stack(gb_system_t *gb)
{
    uint16_t address = gb_read_uint16(gb->sp + 1, gb);

    gb->sp += 2;
    return address;
}

// Call address
void gb_cpu_call(const uint16_t address, gb_system_t *gb)
{
    gb_cpu_push_stack(gb->pc, gb);

    gb->pc = address;
}

// Return
void gb_cpu_ret(gb_system_t *gb)
{
    gb->pc = gb_cpu_pop_stack(gb);
}

// Emulate a GameBoy CPU cycle
int gb_cpu_cycle(gb_system_t *gb)
{
    if (gb->idle_cycles > 0) {
        gb->idle_cycles -= 1;
        return 0;
    }

    byte_t opcode_value = gb_read_byte(gb->pc, gb);
    const gb_opcode_t *opcode = NULL;
    int handler_ret;

    for (uint16_t i = 0; gb_opcode_table[i].handler; ++i) {
        if (gb_opcode_table[i].opcode == opcode_value) {
            opcode = &gb_opcode_table[i];
            break;
        }
    }

    if (opcode) {
        logger(
            LOG_DEBUG,
            "PC: 0x%04X   Opcode: 0x%02X: %s",
            gb->pc,
            opcode_value,
            opcode->mnemonic
        );

        handler_ret = (*opcode->handler)(opcode, gb);
    } else {
        handler_ret = OPCODE_ILLEGAL;
    }

    if (handler_ret == OPCODE_ILLEGAL) {
        logger(
            LOG_CRIT,
            "PC: 0x%04X   Opcode: 0x%02X: Illegal opcode",
            gb->pc,
            opcode_value
        );
        return -1;
    }

    if (handler_ret & OPCODE_ACTION) {
        gb->idle_cycles += opcode->cycles_true - 1;
    } else if (handler_ret & OPCODE_NOACTION) {
        gb->idle_cycles += opcode->cycles_false - 1;
    }

    if (!(handler_ret & OPCODE_NOPC)) {
        gb->pc += opcode->length;
    }

    if (handler_ret & OPCODE_EXIT) {
        return -2;
    }

    return 0;
}