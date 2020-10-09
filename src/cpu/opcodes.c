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
#include "cpu/opcodes/jumps.h"
#include "cpu/opcodes/calls.h"
#include "cpu/opcodes/alu/add.h"
#include "cpu/opcodes/alu/adc.h"

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
        .mnemonic     = "LD BC,nn",
        .opcode       = 0x01,
        .length       = 3,
        .cycles_true  = 12,
        .cycles_false = 12,
        .comment      = "Load nn to BC",
        .handler      = &opcode_ld_r16_nn
    },
    {
        .mnemonic     = "LD (BC),A",
        .opcode       = 0x02,
        .length       = 1,
        .cycles_true  = 8,
        .cycles_false = 8,
        .comment      = "Load A at address BC",
        .handler      = &opcode_ld_a
    },
    {
        .mnemonic     = "LD B,n",
        .opcode       = 0x06,
        .length       = 2,
        .cycles_true  = 8,
        .cycles_false = 8,
        .comment      = "Load n to B",
        .handler      = &opcode_ld_r8
    },
    {
        .mnemonic     = "LD (nn),SP",
        .opcode       = 0x08,
        .length       = 3,
        .cycles_true  = 20,
        .cycles_false = 20,
        .comment      = "Load SP at address nn",
        .handler      = &opcode_ld_sp
    },
    {
        .mnemonic     = "ADD HL,BC",
        .opcode       = 0x09,
        .length       = 1,
        .cycles_true  = 8,
        .cycles_false = 8,
        .comment      = "Add BC to HL",
        .handler      = &opcode_add_hl_n
    },
    {
        .mnemonic     = "LD A,(BC)",
        .opcode       = 0x0A,
        .length       = 1,
        .cycles_true  = 8,
        .cycles_false = 8,
        .comment      = "Load value at address BC to A",
        .handler      = &opcode_ld_a
    },
    {
        .mnemonic     = "LD C,n",
        .opcode       = 0x0E,
        .length       = 2,
        .cycles_true  = 8,
        .cycles_false = 8,
        .comment      = "Load n to C",
        .handler      = &opcode_ld_r8
    },
    {
        .mnemonic     = "LD DE,nn",
        .opcode       = 0x11,
        .length       = 3,
        .cycles_true  = 12,
        .cycles_false = 12,
        .comment      = "Load nn to DE",
        .handler      = &opcode_ld_r16_nn
    },
    {
        .mnemonic     = "LD (DE),A",
        .opcode       = 0x12,
        .length       = 1,
        .cycles_true  = 8,
        .cycles_false = 8,
        .comment      = "Load A at address DE",
        .handler      = &opcode_ld_a
    },
    {
        .mnemonic     = "LD D,n",
        .opcode       = 0x16,
        .length       = 2,
        .cycles_true  = 8,
        .cycles_false = 8,
        .comment      = "Load n to D",
        .handler      = &opcode_ld_r8
    },
    {
        .mnemonic     = "JR n",
        .opcode       = 0x18,
        .length       = 2,
        .cycles_true  = 12,
        .cycles_false = 12,
        .comment      = "Jump to PC+n (signed)",
        .handler      = &opcode_jr
    },
    {
        .mnemonic     = "ADD HL,DE",
        .opcode       = 0x19,
        .length       = 1,
        .cycles_true  = 8,
        .cycles_false = 8,
        .comment      = "Add DE to HL",
        .handler      = &opcode_add_hl_n
    },
    {
        .mnemonic     = "LD A,(DE)",
        .opcode       = 0x1A,
        .length       = 1,
        .cycles_true  = 8,
        .cycles_false = 8,
        .comment      = "Load value at address DE to A",
        .handler      = &opcode_ld_a
    },
    {
        .mnemonic     = "LD E,n",
        .opcode       = 0x1E,
        .length       = 2,
        .cycles_true  = 8,
        .cycles_false = 8,
        .comment      = "Load n to E",
        .handler      = &opcode_ld_r8
    },
    {
        .mnemonic     = "JR NZ,n",
        .opcode       = 0x20,
        .length       = 2,
        .cycles_true  = 12,
        .cycles_false = 8,
        .comment      = "Jump to PC+n (signed) if Z is reset",
        .handler      = &opcode_jr
    },
    {
        .mnemonic     = "LD HL,nn",
        .opcode       = 0x21,
        .length       = 3,
        .cycles_true  = 12,
        .cycles_false = 12,
        .comment      = "Load nn to HL",
        .handler      = &opcode_ld_r16_nn
    },
    {
        .mnemonic     = "LDI (HL),A",
        .opcode       = 0x22,
        .length       = 1,
        .cycles_true  = 8,
        .cycles_false = 8,
        .comment      = "Load A at address HL and increment A",
        .handler      = &opcode_ld_a
    },
    {
        .mnemonic     = "LD H,n",
        .opcode       = 0x26,
        .length       = 2,
        .cycles_true  = 8,
        .cycles_false = 8,
        .comment      = "Load n to H",
        .handler      = &opcode_ld_r8
    },
    {
        .mnemonic     = "JR Z,n",
        .opcode       = 0x28,
        .length       = 2,
        .cycles_true  = 12,
        .cycles_false = 8,
        .comment      = "Jump to PC+n (signed) if Z is set",
        .handler      = &opcode_jr
    },
    {
        .mnemonic     = "ADD HL,HL",
        .opcode       = 0x29,
        .length       = 1,
        .cycles_true  = 8,
        .cycles_false = 8,
        .comment      = "Add HL to HL",
        .handler      = &opcode_add_hl_n
    },
    {
        .mnemonic     = "LDI A,(HL)",
        .opcode       = 0x2A,
        .length       = 1,
        .cycles_true  = 8,
        .cycles_false = 8,
        .comment      = "Load value at address HL to A and increment A",
        .handler      = &opcode_ld_a
    },
    {
        .mnemonic     = "LD L,n",
        .opcode       = 0x2E,
        .length       = 2,
        .cycles_true  = 8,
        .cycles_false = 8,
        .comment      = "Load n to L",
        .handler      = &opcode_ld_r8
    },
    {
        .mnemonic     = "JR NC,n",
        .opcode       = 0x30,
        .length       = 2,
        .cycles_true  = 12,
        .cycles_false = 8,
        .comment      = "Jump to PC+n (signed) if C is reset",
        .handler      = &opcode_jr
    },
    {
        .mnemonic     = "LD SP,nn",
        .opcode       = 0x31,
        .length       = 3,
        .cycles_true  = 12,
        .cycles_false = 12,
        .comment      = "Load nn to SP",
        .handler      = &opcode_ld_sp
    },
    {
        .mnemonic     = "LDD (HL),A",
        .opcode       = 0x32,
        .length       = 1,
        .cycles_true  = 8,
        .cycles_false = 8,
        .comment      = "Load A at address HL and decrement A",
        .handler      = &opcode_ld_a
    },
    {
        .mnemonic     = "JR C,n",
        .opcode       = 0x38,
        .length       = 2,
        .cycles_true  = 12,
        .cycles_false = 8,
        .comment      = "Jump to PC+n (signed) if C is set",
        .handler      = &opcode_jr
    },
    {
        .mnemonic     = "ADD HL,SP",
        .opcode       = 0x39,
        .length       = 1,
        .cycles_true  = 8,
        .cycles_false = 8,
        .comment      = "Add SP to HL",
        .handler      = &opcode_add_hl_n
    },
    {
        .mnemonic     = "LDD A,(HL)",
        .opcode       = 0x3A,
        .length       = 1,
        .cycles_true  = 8,
        .cycles_false = 8,
        .comment      = "Load value at address HL to A and decrement A",
        .handler      = &opcode_ld_a
    },
    {
        .mnemonic     = "LD B,B",
        .opcode       = 0x40,
        .length       = 1,
        .cycles_true  = 4,
        .cycles_false = 4,
        .comment      = "Load B to B",
        .handler      = &opcode_ld_r8
    },
    {
        .mnemonic     = "LD B,C",
        .opcode       = 0x41,
        .length       = 1,
        .cycles_true  = 4,
        .cycles_false = 4,
        .comment      = "Load C to B",
        .handler      = &opcode_ld_r8
    },
    {
        .mnemonic     = "LD B,D",
        .opcode       = 0x42,
        .length       = 1,
        .cycles_true  = 4,
        .cycles_false = 4,
        .comment      = "Load D to B",
        .handler      = &opcode_ld_r8
    },
    {
        .mnemonic     = "LD B,E",
        .opcode       = 0x43,
        .length       = 1,
        .cycles_true  = 4,
        .cycles_false = 4,
        .comment      = "Load E to B",
        .handler      = &opcode_ld_r8
    },
    {
        .mnemonic     = "LD B,H",
        .opcode       = 0x44,
        .length       = 1,
        .cycles_true  = 4,
        .cycles_false = 4,
        .comment      = "Load H to B",
        .handler      = &opcode_ld_r8
    },
    {
        .mnemonic     = "LD B,L",
        .opcode       = 0x45,
        .length       = 1,
        .cycles_true  = 4,
        .cycles_false = 4,
        .comment      = "Load L to B",
        .handler      = &opcode_ld_r8
    },
    {
        .mnemonic     = "LD B,(HL)",
        .opcode       = 0x46,
        .length       = 1,
        .cycles_true  = 8,
        .cycles_false = 8,
        .comment      = "Load value at address HL to B",
        .handler      = &opcode_ld_r8
    },
    {
        .mnemonic     = "LD B,A",
        .opcode       = 0x47,
        .length       = 1,
        .cycles_true  = 4,
        .cycles_false = 4,
        .comment      = "Load A to B",
        .handler      = &opcode_ld_a
    },
    {
        .mnemonic     = "LD C,B",
        .opcode       = 0x48,
        .length       = 1,
        .cycles_true  = 4,
        .cycles_false = 4,
        .comment      = "Load B to C",
        .handler      = &opcode_ld_r8
    },
    {
        .mnemonic     = "LD C,C",
        .opcode       = 0x49,
        .length       = 1,
        .cycles_true  = 4,
        .cycles_false = 4,
        .comment      = "Load C to C",
        .handler      = &opcode_ld_r8
    },
    {
        .mnemonic     = "LD C,D",
        .opcode       = 0x4A,
        .length       = 1,
        .cycles_true  = 4,
        .cycles_false = 4,
        .comment      = "Load D to C",
        .handler      = &opcode_ld_r8
    },
    {
        .mnemonic     = "LD C,E",
        .opcode       = 0x4B,
        .length       = 1,
        .cycles_true  = 4,
        .cycles_false = 4,
        .comment      = "Load E to C",
        .handler      = &opcode_ld_r8
    },
    {
        .mnemonic     = "LD C,H",
        .opcode       = 0x4C,
        .length       = 1,
        .cycles_true  = 4,
        .cycles_false = 4,
        .comment      = "Load H to C",
        .handler      = &opcode_ld_r8
    },
    {
        .mnemonic     = "LD C,L",
        .opcode       = 0x4D,
        .length       = 1,
        .cycles_true  = 4,
        .cycles_false = 4,
        .comment      = "Load L to C",
        .handler      = &opcode_ld_r8
    },
    {
        .mnemonic     = "LD C,(HL)",
        .opcode       = 0x4E,
        .length       = 1,
        .cycles_true  = 8,
        .cycles_false = 8,
        .comment      = "Load value at address HL to C",
        .handler      = &opcode_ld_r8
    },
    {
        .mnemonic     = "LD C,A",
        .opcode       = 0x4F,
        .length       = 1,
        .cycles_true  = 4,
        .cycles_false = 4,
        .comment      = "Load A to C",
        .handler      = &opcode_ld_a
    },
    {
        .mnemonic     = "LD D,B",
        .opcode       = 0x50,
        .length       = 1,
        .cycles_true  = 4,
        .cycles_false = 4,
        .comment      = "Load B to D",
        .handler      = &opcode_ld_r8
    },
    {
        .mnemonic     = "LD D,C",
        .opcode       = 0x51,
        .length       = 1,
        .cycles_true  = 4,
        .cycles_false = 4,
        .comment      = "Load C to D",
        .handler      = &opcode_ld_r8
    },
    {
        .mnemonic     = "LD D,D",
        .opcode       = 0x52,
        .length       = 1,
        .cycles_true  = 4,
        .cycles_false = 4,
        .comment      = "Load D to D",
        .handler      = &opcode_ld_r8
    },
    {
        .mnemonic     = "LD D,E",
        .opcode       = 0x53,
        .length       = 1,
        .cycles_true  = 4,
        .cycles_false = 4,
        .comment      = "Load E to D",
        .handler      = &opcode_ld_r8
    },
    {
        .mnemonic     = "LD D,H",
        .opcode       = 0x54,
        .length       = 1,
        .cycles_true  = 4,
        .cycles_false = 4,
        .comment      = "Load H to D",
        .handler      = &opcode_ld_r8
    },
    {
        .mnemonic     = "LD D,L",
        .opcode       = 0x55,
        .length       = 1,
        .cycles_true  = 4,
        .cycles_false = 4,
        .comment      = "Load L to D",
        .handler      = &opcode_ld_r8
    },
    {
        .mnemonic     = "LD D,(HL)",
        .opcode       = 0x56,
        .length       = 1,
        .cycles_true  = 8,
        .cycles_false = 8,
        .comment      = "Load value at address HL to D",
        .handler      = &opcode_ld_r8
    },
    {
        .mnemonic     = "LD D,A",
        .opcode       = 0x57,
        .length       = 1,
        .cycles_true  = 4,
        .cycles_false = 4,
        .comment      = "Load A to D",
        .handler      = &opcode_ld_a
    },
    {
        .mnemonic     = "LD E,B",
        .opcode       = 0x58,
        .length       = 1,
        .cycles_true  = 4,
        .cycles_false = 4,
        .comment      = "Load B to E",
        .handler      = &opcode_ld_r8
    },
    {
        .mnemonic     = "LD E,C",
        .opcode       = 0x59,
        .length       = 1,
        .cycles_true  = 4,
        .cycles_false = 4,
        .comment      = "Load C to E",
        .handler      = &opcode_ld_r8
    },
    {
        .mnemonic     = "LD E,D",
        .opcode       = 0x5A,
        .length       = 1,
        .cycles_true  = 4,
        .cycles_false = 4,
        .comment      = "Load D to E",
        .handler      = &opcode_ld_r8
    },
    {
        .mnemonic     = "LD E,E",
        .opcode       = 0x5B,
        .length       = 1,
        .cycles_true  = 4,
        .cycles_false = 4,
        .comment      = "Load E to E",
        .handler      = &opcode_ld_r8
    },
    {
        .mnemonic     = "LD E,H",
        .opcode       = 0x5C,
        .length       = 1,
        .cycles_true  = 4,
        .cycles_false = 4,
        .comment      = "Load H to E",
        .handler      = &opcode_ld_r8
    },
    {
        .mnemonic     = "LD E,L",
        .opcode       = 0x5D,
        .length       = 1,
        .cycles_true  = 4,
        .cycles_false = 4,
        .comment      = "Load L to E",
        .handler      = &opcode_ld_r8
    },
    {
        .mnemonic     = "LD E,(HL)",
        .opcode       = 0x5E,
        .length       = 1,
        .cycles_true  = 8,
        .cycles_false = 8,
        .comment      = "Load value at address HL to E",
        .handler      = &opcode_ld_r8
    },
    {
        .mnemonic     = "LD E,A",
        .opcode       = 0x5F,
        .length       = 1,
        .cycles_true  = 4,
        .cycles_false = 4,
        .comment      = "Load A to E",
        .handler      = &opcode_ld_a
    },
    {
        .mnemonic     = "LD H,B",
        .opcode       = 0x60,
        .length       = 1,
        .cycles_true  = 4,
        .cycles_false = 4,
        .comment      = "Load B to H",
        .handler      = &opcode_ld_r8
    },
    {
        .mnemonic     = "LD H,C",
        .opcode       = 0x61,
        .length       = 1,
        .cycles_true  = 4,
        .cycles_false = 4,
        .comment      = "Load C to H",
        .handler      = &opcode_ld_r8
    },
    {
        .mnemonic     = "LD H,D",
        .opcode       = 0x62,
        .length       = 1,
        .cycles_true  = 4,
        .cycles_false = 4,
        .comment      = "Load D to H",
        .handler      = &opcode_ld_r8
    },
    {
        .mnemonic     = "LD H,E",
        .opcode       = 0x63,
        .length       = 1,
        .cycles_true  = 4,
        .cycles_false = 4,
        .comment      = "Load E to H",
        .handler      = &opcode_ld_r8
    },
    {
        .mnemonic     = "LD H,H",
        .opcode       = 0x64,
        .length       = 1,
        .cycles_true  = 4,
        .cycles_false = 4,
        .comment      = "Load H to H",
        .handler      = &opcode_ld_r8
    },
    {
        .mnemonic     = "LD H,L",
        .opcode       = 0x65,
        .length       = 1,
        .cycles_true  = 4,
        .cycles_false = 4,
        .comment      = "Load L to H",
        .handler      = &opcode_ld_r8
    },
    {
        .mnemonic     = "LD H,(HL)",
        .opcode       = 0x66,
        .length       = 1,
        .cycles_true  = 8,
        .cycles_false = 8,
        .comment      = "Load value at address HL to H",
        .handler      = &opcode_ld_r8
    },
    {
        .mnemonic     = "LD H,A",
        .opcode       = 0x67,
        .length       = 1,
        .cycles_true  = 4,
        .cycles_false = 4,
        .comment      = "Load A to H",
        .handler      = &opcode_ld_a
    },
    {
        .mnemonic     = "LD L,B",
        .opcode       = 0x68,
        .length       = 1,
        .cycles_true  = 4,
        .cycles_false = 4,
        .comment      = "Load B to L",
        .handler      = &opcode_ld_r8
    },
    {
        .mnemonic     = "LD L,C",
        .opcode       = 0x69,
        .length       = 1,
        .cycles_true  = 4,
        .cycles_false = 4,
        .comment      = "Load C to L",
        .handler      = &opcode_ld_r8
    },
    {
        .mnemonic     = "LD L,D",
        .opcode       = 0x6A,
        .length       = 1,
        .cycles_true  = 4,
        .cycles_false = 4,
        .comment      = "Load D to L",
        .handler      = &opcode_ld_r8
    },
    {
        .mnemonic     = "LD L,E",
        .opcode       = 0x6B,
        .length       = 1,
        .cycles_true  = 4,
        .cycles_false = 4,
        .comment      = "Load E to L",
        .handler      = &opcode_ld_r8
    },
    {
        .mnemonic     = "LD L,H",
        .opcode       = 0x6C,
        .length       = 1,
        .cycles_true  = 4,
        .cycles_false = 4,
        .comment      = "Load H to L",
        .handler      = &opcode_ld_r8
    },
    {
        .mnemonic     = "LD L,L",
        .opcode       = 0x6D,
        .length       = 1,
        .cycles_true  = 4,
        .cycles_false = 4,
        .comment      = "Load L to L",
        .handler      = &opcode_ld_r8
    },
    {
        .mnemonic     = "LD L,(HL)",
        .opcode       = 0x6E,
        .length       = 1,
        .cycles_true  = 8,
        .cycles_false = 8,
        .comment      = "Load value at address HL to L",
        .handler      = &opcode_ld_r8
    },
    {
        .mnemonic     = "LD L,A",
        .opcode       = 0x6F,
        .length       = 1,
        .cycles_true  = 4,
        .cycles_false = 4,
        .comment      = "Load A to L",
        .handler      = &opcode_ld_a
    },
    {
        .mnemonic     = "LD (HL),B",
        .opcode       = 0x70,
        .length       = 1,
        .cycles_true  = 8,
        .cycles_false = 8,
        .comment      = "Load B at address HL",
        .handler      = &opcode_ld_r8
    },
    {
        .mnemonic     = "LD (HL),C",
        .opcode       = 0x71,
        .length       = 1,
        .cycles_true  = 8,
        .cycles_false = 8,
        .comment      = "Load C at address HL",
        .handler      = &opcode_ld_r8
    },
    {
        .mnemonic     = "LD (HL),D",
        .opcode       = 0x72,
        .length       = 1,
        .cycles_true  = 8,
        .cycles_false = 8,
        .comment      = "Load D at address HL",
        .handler      = &opcode_ld_r8
    },
    {
        .mnemonic     = "LD (HL),E",
        .opcode       = 0x73,
        .length       = 1,
        .cycles_true  = 8,
        .cycles_false = 8,
        .comment      = "Load E at address HL",
        .handler      = &opcode_ld_r8
    },
    {
        .mnemonic     = "LD (HL),H",
        .opcode       = 0x74,
        .length       = 1,
        .cycles_true  = 8,
        .cycles_false = 8,
        .comment      = "Load H at address HL",
        .handler      = &opcode_ld_r8
    },
    {
        .mnemonic     = "LD (HL),L",
        .opcode       = 0x75,
        .length       = 1,
        .cycles_true  = 8,
        .cycles_false = 8,
        .comment      = "Load L at address HL",
        .handler      = &opcode_ld_r8
    },
    {
        .mnemonic     = "LD (HL),A",
        .opcode       = 0x77,
        .length       = 1,
        .cycles_true  = 8,
        .cycles_false = 8,
        .comment      = "Load A at address HL",
        .handler      = &opcode_ld_a
    },
    {
        .mnemonic     = "LD A,B",
        .opcode       = 0x78,
        .length       = 1,
        .cycles_true  = 4,
        .cycles_false = 4,
        .comment      = "Load B to A",
        .handler      = &opcode_ld_a
    },
    {
        .mnemonic     = "LD A,C",
        .opcode       = 0x79,
        .length       = 1,
        .cycles_true  = 4,
        .cycles_false = 4,
        .comment      = "Load C to A",
        .handler      = &opcode_ld_a
    },
    {
        .mnemonic     = "LD A,D",
        .opcode       = 0x7A,
        .length       = 1,
        .cycles_true  = 4,
        .cycles_false = 4,
        .comment      = "Load D to A",
        .handler      = &opcode_ld_a
    },
    {
        .mnemonic     = "LD A,E",
        .opcode       = 0x7B,
        .length       = 1,
        .cycles_true  = 4,
        .cycles_false = 4,
        .comment      = "Load E to A",
        .handler      = &opcode_ld_a
    },
    {
        .mnemonic     = "LD A,H",
        .opcode       = 0x7C,
        .length       = 1,
        .cycles_true  = 4,
        .cycles_false = 4,
        .comment      = "Load H to A",
        .handler      = &opcode_ld_a
    },
    {
        .mnemonic     = "LD A,L",
        .opcode       = 0x7D,
        .length       = 1,
        .cycles_true  = 4,
        .cycles_false = 4,
        .comment      = "Load L to A",
        .handler      = &opcode_ld_a
    },
    {
        .mnemonic     = "LD A,(HL)",
        .opcode       = 0x7E,
        .length       = 1,
        .cycles_true  = 8,
        .cycles_false = 8,
        .comment      = "Load value at address HL to A",
        .handler      = &opcode_ld_a
    },
    {
        .mnemonic     = "LD A,A",
        .opcode       = 0x7F,
        .length       = 1,
        .cycles_true  = 4,
        .cycles_false = 4,
        .comment      = "Load A to A",
        .handler      = &opcode_ld_a
    },
    {
        .mnemonic     = "ADD A,B",
        .opcode       = 0x80,
        .length       = 1,
        .cycles_true  = 4,
        .cycles_false = 4,
        .comment      = "Add B to A",
        .handler      = &opcode_add_a_n
    },
    {
        .mnemonic     = "ADD A,C",
        .opcode       = 0x81,
        .length       = 1,
        .cycles_true  = 4,
        .cycles_false = 4,
        .comment      = "Add C to A",
        .handler      = &opcode_add_a_n
    },
    {
        .mnemonic     = "ADD A,D",
        .opcode       = 0x82,
        .length       = 1,
        .cycles_true  = 4,
        .cycles_false = 4,
        .comment      = "Add D to A",
        .handler      = &opcode_add_a_n
    },
    {
        .mnemonic     = "ADD A,E",
        .opcode       = 0x83,
        .length       = 1,
        .cycles_true  = 4,
        .cycles_false = 4,
        .comment      = "Add E to A",
        .handler      = &opcode_add_a_n
    },
    {
        .mnemonic     = "ADD A,H",
        .opcode       = 0x84,
        .length       = 1,
        .cycles_true  = 4,
        .cycles_false = 4,
        .comment      = "Add H to A",
        .handler      = &opcode_add_a_n
    },
    {
        .mnemonic     = "ADD A,L",
        .opcode       = 0x85,
        .length       = 1,
        .cycles_true  = 4,
        .cycles_false = 4,
        .comment      = "Add L to A",
        .handler      = &opcode_add_a_n
    },
    {
        .mnemonic     = "ADD A,(HL)",
        .opcode       = 0x86,
        .length       = 1,
        .cycles_true  = 8,
        .cycles_false = 8,
        .comment      = "Add value at address stored in HL to A",
        .handler      = &opcode_add_a_n
    },
    {
        .mnemonic     = "ADD A,A",
        .opcode       = 0x87,
        .length       = 1,
        .cycles_true  = 4,
        .cycles_false = 4,
        .comment      = "Add A to A",
        .handler      = &opcode_add_a_n
    },
    {
        .mnemonic     = "ADC A,B",
        .opcode       = 0x88,
        .length       = 1,
        .cycles_true  = 4,
        .cycles_false = 4,
        .comment      = "Add B+Cy to A",
        .handler      = &opcode_adc_a_n
    },
    {
        .mnemonic     = "ADC A,C",
        .opcode       = 0x89,
        .length       = 1,
        .cycles_true  = 4,
        .cycles_false = 4,
        .comment      = "Add C+Cy to A",
        .handler      = &opcode_adc_a_n
    },
    {
        .mnemonic     = "ADC A,D",
        .opcode       = 0x8A,
        .length       = 1,
        .cycles_true  = 4,
        .cycles_false = 4,
        .comment      = "Add D+Cy to A",
        .handler      = &opcode_adc_a_n
    },
    {
        .mnemonic     = "ADC A,E",
        .opcode       = 0x8B,
        .length       = 1,
        .cycles_true  = 4,
        .cycles_false = 4,
        .comment      = "Add E+Cy to A",
        .handler      = &opcode_adc_a_n
    },
    {
        .mnemonic     = "ADC A,H",
        .opcode       = 0x8C,
        .length       = 1,
        .cycles_true  = 4,
        .cycles_false = 4,
        .comment      = "Add H+Cy to A",
        .handler      = &opcode_adc_a_n
    },
    {
        .mnemonic     = "ADC A,L",
        .opcode       = 0x8D,
        .length       = 1,
        .cycles_true  = 4,
        .cycles_false = 4,
        .comment      = "Add L+Cy to A",
        .handler      = &opcode_adc_a_n
    },
    {
        .mnemonic     = "ADC A,(HL)",
        .opcode       = 0x8E,
        .length       = 1,
        .cycles_true  = 8,
        .cycles_false = 8,
        .comment      = "Add (HL)+Cy to A",
        .handler      = &opcode_adc_a_n
    },
    {
        .mnemonic     = "ADC A,A",
        .opcode       = 0x8F,
        .length       = 1,
        .cycles_true  = 4,
        .cycles_false = 4,
        .comment      = "Add A+Cy to A",
        .handler      = &opcode_adc_a_n
    },
    {
        .mnemonic     = "RET NZ",
        .opcode       = 0xC0,
        .length       = 1,
        .cycles_true  = 20,
        .cycles_false = 8,
        .comment      = "Pop address from stack and jump to it if Z is reset",
        .handler      = &opcode_ret
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
        .mnemonic     = "JP NZ,nn",
        .opcode       = 0xC2,
        .length       = 3,
        .cycles_true  = 16,
        .cycles_false = 12,
        .comment      = "Jump to nn if Z if reset",
        .handler      = &opcode_jp
    },
    {
        .mnemonic     = "JP nn",
        .opcode       = 0xC3,
        .length       = 3,
        .cycles_true  = 16,
        .cycles_false = 16,
        .comment      = "Jump to nn",
        .handler      = &opcode_jp
    },
    {
        .mnemonic     = "CALL NZ,nn",
        .opcode       = 0xC4,
        .length       = 3,
        .cycles_true  = 24,
        .cycles_false = 12,
        .comment      = "Push PC and jump to nn if Z if reset",
        .handler      = &opcode_call
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
        .mnemonic     = "ADD A,n",
        .opcode       = 0xC6,
        .length       = 2,
        .cycles_true  = 8,
        .cycles_false = 8,
        .comment      = "Add n to A",
        .handler      = &opcode_add_a_n
    },
    {
        .mnemonic     = "RST $00",
        .opcode       = 0xC7,
        .length       = 1,
        .cycles_true  = 16,
        .cycles_false = 16,
        .comment      = "Call $00",
        .handler      = &opcode_rst
    },
    {
        .mnemonic     = "RET Z",
        .opcode       = 0xC8,
        .length       = 1,
        .cycles_true  = 20,
        .cycles_false = 8,
        .comment      = "Pop address from stack and jump to it if Z is set",
        .handler      = &opcode_ret
    },
    {
        .mnemonic     = "RET",
        .opcode       = 0xC9,
        .length       = 1,
        .cycles_true  = 16,
        .cycles_false = 16,
        .comment      = "Pop address from stack and jump to it",
        .handler      = &opcode_ret
    },
    {
        .mnemonic     = "JP Z,nn",
        .opcode       = 0xCA,
        .length       = 3,
        .cycles_true  = 16,
        .cycles_false = 12,
        .comment      = "Jump to nn if Z if set",
        .handler      = &opcode_jp
    },
    {
        .mnemonic     = "CALL Z,nn",
        .opcode       = 0xCC,
        .length       = 3,
        .cycles_true  = 24,
        .cycles_false = 12,
        .comment      = "Push PC and jump to nn if Z if set",
        .handler      = &opcode_call
    },
    {
        .mnemonic     = "CALL nn",
        .opcode       = 0xCD,
        .length       = 3,
        .cycles_true  = 24,
        .cycles_false = 24,
        .comment      = "Push PC and jump to nn",
        .handler      = &opcode_call
    },
    {
        .mnemonic     = "ADC A,n",
        .opcode       = 0xCE,
        .length       = 2,
        .cycles_true  = 8,
        .cycles_false = 8,
        .comment      = "Add n+Cy to A",
        .handler      = &opcode_adc_a_n
    },
    {
        .mnemonic     = "RST $08",
        .opcode       = 0xCF,
        .length       = 1,
        .cycles_true  = 16,
        .cycles_false = 16,
        .comment      = "Call $08",
        .handler      = &opcode_rst
    },
    {
        .mnemonic     = "RET NC",
        .opcode       = 0xD0,
        .length       = 1,
        .cycles_true  = 20,
        .cycles_false = 8,
        .comment      = "Pop address from stack and jump to it if C is reset",
        .handler      = &opcode_ret
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
        .mnemonic     = "JP NC,nn",
        .opcode       = 0xD2,
        .length       = 3,
        .cycles_true  = 16,
        .cycles_false = 12,
        .comment      = "Jump to nn if C if reset",
        .handler      = &opcode_jp
    },
    {
        .mnemonic     = "CALL NC,nn",
        .opcode       = 0xD4,
        .length       = 3,
        .cycles_true  = 24,
        .cycles_false = 12,
        .comment      = "Push PC and jump to nn if C if reset",
        .handler      = &opcode_call
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
        .mnemonic     = "RST $10",
        .opcode       = 0xD7,
        .length       = 1,
        .cycles_true  = 16,
        .cycles_false = 16,
        .comment      = "Call $10",
        .handler      = &opcode_rst
    },
    {
        .mnemonic     = "RET C",
        .opcode       = 0xD8,
        .length       = 1,
        .cycles_true  = 20,
        .cycles_false = 8,
        .comment      = "Pop address from stack and jump to it if C is set",
        .handler      = &opcode_ret
    },
    {
        .mnemonic     = "RETI",
        .opcode       = 0xD9,
        .length       = 1,
        .cycles_true  = 16,
        .cycles_false = 16,
        .comment      = "Pop address from stack, jump to it and enable interrupts",
        .handler      = &opcode_reti
    },
    {
        .mnemonic     = "JP C,nn",
        .opcode       = 0xDA,
        .length       = 3,
        .cycles_true  = 16,
        .cycles_false = 12,
        .comment      = "Jump to nn if C if set",
        .handler      = &opcode_jp
    },
    {
        .mnemonic     = "CALL C,nn",
        .opcode       = 0xDC,
        .length       = 3,
        .cycles_true  = 24,
        .cycles_false = 12,
        .comment      = "Push PC and jump to nn if C if set",
        .handler      = &opcode_call
    },
    {
        .mnemonic     = "RST $18",
        .opcode       = 0xDF,
        .length       = 1,
        .cycles_true  = 16,
        .cycles_false = 16,
        .comment      = "Call $18",
        .handler      = &opcode_rst
    },
    {
        .mnemonic     = "LDH (n),A",
        .opcode       = 0xE0,
        .length       = 2,
        .cycles_true  = 12,
        .cycles_false = 12,
        .comment      = "Load A at address $FF00+n",
        .handler      = &opcode_ld_a
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
        .mnemonic     = "LD (C),A",
        .opcode       = 0xE2,
        .length       = 2,
        .cycles_true  = 8,
        .cycles_false = 8,
        .comment      = "Load A at address $FF00+C",
        .handler      = &opcode_ld_a
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
        .mnemonic     = "RST $20",
        .opcode       = 0xE7,
        .length       = 1,
        .cycles_true  = 16,
        .cycles_false = 16,
        .comment      = "Call $20",
        .handler      = &opcode_rst
    },
    {
        .mnemonic     = "JP (HL)",
        .opcode       = 0xE9,
        .length       = 1,
        .cycles_true  = 4,
        .cycles_false = 4,
        .comment      = "Jump to address in HL",
        .handler      = &opcode_jp_hl
    },
    {
        .mnemonic     = "LD (nn),A",
        .opcode       = 0xEA,
        .length       = 3,
        .cycles_true  = 16,
        .cycles_false = 16,
        .comment      = "Load A at address nn",
        .handler      = &opcode_ld_a
    },
    {
        .mnemonic     = "RST $28",
        .opcode       = 0xEF,
        .length       = 1,
        .cycles_true  = 16,
        .cycles_false = 16,
        .comment      = "Call $28",
        .handler      = &opcode_rst
    },
    {
        .mnemonic     = "LDH A,(n)",
        .opcode       = 0xF0,
        .length       = 2,
        .cycles_true  = 12,
        .cycles_false = 12,
        .comment      = "Load value at address $FF00+n to A",
        .handler      = &opcode_ld_a
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
        .mnemonic     = "LD A,(C)",
        .opcode       = 0xF2,
        .length       = 2,
        .cycles_true  = 8,
        .cycles_false = 8,
        .comment      = "Load value at address $FF00+C to A",
        .handler      = &opcode_ld_a
    },
    {
        .mnemonic     = "DI",
        .opcode       = 0xF3,
        .length       = 1,
        .cycles_true  = 4,
        .cycles_false = 4,
        .comment      = "Disable Interrupts",
        .handler      = &opcode_di
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
    {
        .mnemonic     = "RST $30",
        .opcode       = 0xF7,
        .length       = 1,
        .cycles_true  = 16,
        .cycles_false = 16,
        .comment      = "Call $30",
        .handler      = &opcode_rst
    },
    {
        .mnemonic     = "LD SP,HL",
        .opcode       = 0xF9,
        .length       = 1,
        .cycles_true  = 8,
        .cycles_false = 8,
        .comment      = "Load HL to SP",
        .handler      = &opcode_ld_sp
    },
    {
        .mnemonic     = "LD A,(nn)",
        .opcode       = 0xFA,
        .length       = 3,
        .cycles_true  = 16,
        .cycles_false = 16,
        .comment      = "Load value at address nn to A",
        .handler      = &opcode_ld_a
    },
    {
        .mnemonic     = "EI",
        .opcode       = 0xFB,
        .length       = 1,
        .cycles_true  = 4,
        .cycles_false = 4,
        .comment      = "Enable Interrupts",
        .handler      = &opcode_ei
    },
    {
        .mnemonic     = "RST $FF",
        .opcode       = 0xFF,
        .length       = 1,
        .cycles_true  = 16,
        .cycles_false = 16,
        .comment      = "Call $FF",
        .handler      = &opcode_rst
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