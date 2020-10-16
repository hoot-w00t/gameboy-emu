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
#include "cpu/opcodes/alu/sub.h"
#include "cpu/opcodes/alu/sbc.h"
#include "cpu/opcodes/alu/and.h"
#include "cpu/opcodes/alu/xor.h"
#include "cpu/opcodes/alu/or.h"
#include "cpu/opcodes/alu/cp.h"
#include "cpu/opcodes/alu/inc.h"
#include "cpu/opcodes/alu/dec.h"

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
        .mnemonic     = "INC BC",
        .opcode       = 0x03,
        .length       = 1,
        .cycles_true  = 8,
        .cycles_false = 8,
        .comment      = "Increment BC",
        .handler      = &opcode_inc_nn
    },
    {
        .mnemonic     = "INC B",
        .opcode       = 0x04,
        .length       = 1,
        .cycles_true  = 4,
        .cycles_false = 4,
        .comment      = "Increment B",
        .handler      = &opcode_inc_n_r
    },
    {
        .mnemonic     = "DEC B",
        .opcode       = 0x05,
        .length       = 1,
        .cycles_true  = 4,
        .cycles_false = 4,
        .comment      = "Decrement B",
        .handler      = &opcode_dec_n_r
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
        .mnemonic     = "DEC BC",
        .opcode       = 0x0B,
        .length       = 1,
        .cycles_true  = 8,
        .cycles_false = 8,
        .comment      = "Decrement BC",
        .handler      = &opcode_dec_nn
    },
    {
        .mnemonic     = "INC C",
        .opcode       = 0x0C,
        .length       = 1,
        .cycles_true  = 4,
        .cycles_false = 4,
        .comment      = "Increment C",
        .handler      = &opcode_inc_n_r
    },
    {
        .mnemonic     = "DEC C",
        .opcode       = 0x0D,
        .length       = 1,
        .cycles_true  = 4,
        .cycles_false = 4,
        .comment      = "Decrement C",
        .handler      = &opcode_dec_n_r
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
        .mnemonic     = "INC DE",
        .opcode       = 0x13,
        .length       = 1,
        .cycles_true  = 8,
        .cycles_false = 8,
        .comment      = "Increment DE",
        .handler      = &opcode_inc_nn
    },
    {
        .mnemonic     = "INC D",
        .opcode       = 0x14,
        .length       = 1,
        .cycles_true  = 4,
        .cycles_false = 4,
        .comment      = "Increment D",
        .handler      = &opcode_inc_n_r
    },
    {
        .mnemonic     = "DEC D",
        .opcode       = 0x15,
        .length       = 1,
        .cycles_true  = 4,
        .cycles_false = 4,
        .comment      = "Decrement D",
        .handler      = &opcode_dec_n_r
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
        .mnemonic     = "DEC DE",
        .opcode       = 0x1B,
        .length       = 1,
        .cycles_true  = 8,
        .cycles_false = 8,
        .comment      = "Decrement DE",
        .handler      = &opcode_dec_nn
    },
    {
        .mnemonic     = "INC E",
        .opcode       = 0x1C,
        .length       = 1,
        .cycles_true  = 4,
        .cycles_false = 4,
        .comment      = "Increment E",
        .handler      = &opcode_inc_n_r
    },
    {
        .mnemonic     = "DEC E",
        .opcode       = 0x1D,
        .length       = 1,
        .cycles_true  = 4,
        .cycles_false = 4,
        .comment      = "Decrement E",
        .handler      = &opcode_dec_n_r
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
        .mnemonic     = "INC HL",
        .opcode       = 0x23,
        .length       = 1,
        .cycles_true  = 8,
        .cycles_false = 8,
        .comment      = "Increment HL",
        .handler      = &opcode_inc_nn
    },
    {
        .mnemonic     = "INC H",
        .opcode       = 0x24,
        .length       = 1,
        .cycles_true  = 4,
        .cycles_false = 4,
        .comment      = "Increment H",
        .handler      = &opcode_inc_n_r
    },
    {
        .mnemonic     = "DEC H",
        .opcode       = 0x25,
        .length       = 1,
        .cycles_true  = 4,
        .cycles_false = 4,
        .comment      = "Decrement H",
        .handler      = &opcode_dec_n_r
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
        .mnemonic     = "DAA",
        .opcode       = 0x27,
        .length       = 1,
        .cycles_true  = 4,
        .cycles_false = 4,
        .comment      = "Decimal adjust A",
        .handler      = &opcode_daa
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
        .mnemonic     = "DEC HL",
        .opcode       = 0x2B,
        .length       = 1,
        .cycles_true  = 8,
        .cycles_false = 8,
        .comment      = "Decrement HL",
        .handler      = &opcode_dec_nn
    },
    {
        .mnemonic     = "INC L",
        .opcode       = 0x2C,
        .length       = 1,
        .cycles_true  = 4,
        .cycles_false = 4,
        .comment      = "Increment L",
        .handler      = &opcode_inc_n_r
    },
    {
        .mnemonic     = "DEC L",
        .opcode       = 0x2D,
        .length       = 1,
        .cycles_true  = 4,
        .cycles_false = 4,
        .comment      = "Decrement L",
        .handler      = &opcode_dec_n_r
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
        .mnemonic     = "INC SP",
        .opcode       = 0x33,
        .length       = 1,
        .cycles_true  = 8,
        .cycles_false = 8,
        .comment      = "Increment SP",
        .handler      = &opcode_inc_nn
    },
    {
        .mnemonic     = "INC (HL)",
        .opcode       = 0x34,
        .length       = 1,
        .cycles_true  = 12,
        .cycles_false = 12,
        .comment      = "Increment (HL)",
        .handler      = &opcode_inc_n_hl
    },
    {
        .mnemonic     = "DEC (HL)",
        .opcode       = 0x35,
        .length       = 1,
        .cycles_true  = 12,
        .cycles_false = 12,
        .comment      = "Decrement (HL)",
        .handler      = &opcode_dec_n_hl
    },
    {
        .mnemonic     = "LD (HL),n",
        .opcode       = 0x36,
        .length       = 2,
        .cycles_true  = 12,
        .cycles_false = 12,
        .comment      = "Load n to (HL)",
        .handler      = &opcode_ld_r8
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
        .mnemonic     = "DEC SP",
        .opcode       = 0x3B,
        .length       = 1,
        .cycles_true  = 8,
        .cycles_false = 8,
        .comment      = "Decrement SP",
        .handler      = &opcode_dec_nn
    },
    {
        .mnemonic     = "INC A",
        .opcode       = 0x3C,
        .length       = 1,
        .cycles_true  = 4,
        .cycles_false = 4,
        .comment      = "Increment A",
        .handler      = &opcode_inc_n_r
    },
    {
        .mnemonic     = "DEC A",
        .opcode       = 0x3D,
        .length       = 1,
        .cycles_true  = 4,
        .cycles_false = 4,
        .comment      = "Decrement A",
        .handler      = &opcode_dec_n_r
    },
    {
        .mnemonic     = "LD A,n",
        .opcode       = 0x3E,
        .length       = 2,
        .cycles_true  = 8,
        .cycles_false = 8,
        .comment      = "Load n to A",
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
        .mnemonic     = "SUB A,B",
        .opcode       = 0x90,
        .length       = 1,
        .cycles_true  = 4,
        .cycles_false = 4,
        .comment      = "Subtract B from A",
        .handler      = &opcode_sub
    },
    {
        .mnemonic     = "SUB A,C",
        .opcode       = 0x91,
        .length       = 1,
        .cycles_true  = 4,
        .cycles_false = 4,
        .comment      = "Subtract C from A",
        .handler      = &opcode_sub
    },
    {
        .mnemonic     = "SUB A,D",
        .opcode       = 0x92,
        .length       = 1,
        .cycles_true  = 4,
        .cycles_false = 4,
        .comment      = "Subtract D from A",
        .handler      = &opcode_sub
    },
    {
        .mnemonic     = "SUB A,E",
        .opcode       = 0x93,
        .length       = 1,
        .cycles_true  = 4,
        .cycles_false = 4,
        .comment      = "Subtract E from A",
        .handler      = &opcode_sub
    },
    {
        .mnemonic     = "SUB A,H",
        .opcode       = 0x94,
        .length       = 1,
        .cycles_true  = 4,
        .cycles_false = 4,
        .comment      = "Subtract H from A",
        .handler      = &opcode_sub
    },
    {
        .mnemonic     = "SUB A,L",
        .opcode       = 0x95,
        .length       = 1,
        .cycles_true  = 4,
        .cycles_false = 4,
        .comment      = "Subtract L from A",
        .handler      = &opcode_sub
    },
    {
        .mnemonic     = "SUB A,(HL)",
        .opcode       = 0x96,
        .length       = 1,
        .cycles_true  = 8,
        .cycles_false = 8,
        .comment      = "Subtract (HL) from A",
        .handler      = &opcode_sub
    },
    {
        .mnemonic     = "SUB A,A",
        .opcode       = 0x97,
        .length       = 1,
        .cycles_true  = 4,
        .cycles_false = 4,
        .comment      = "Subtract A from A",
        .handler      = &opcode_sub
    },
    {
        .mnemonic     = "SBC A,B",
        .opcode       = 0x98,
        .length       = 1,
        .cycles_true  = 4,
        .cycles_false = 4,
        .comment      = "Subtract B+Cy from A",
        .handler      = &opcode_sbc
    },
    {
        .mnemonic     = "SBC A,C",
        .opcode       = 0x99,
        .length       = 1,
        .cycles_true  = 4,
        .cycles_false = 4,
        .comment      = "Subtract C+Cy from A",
        .handler      = &opcode_sbc
    },
    {
        .mnemonic     = "SBC A,D",
        .opcode       = 0x9A,
        .length       = 1,
        .cycles_true  = 4,
        .cycles_false = 4,
        .comment      = "Subtract D+Cy from A",
        .handler      = &opcode_sbc
    },
    {
        .mnemonic     = "SBC A,E",
        .opcode       = 0x9B,
        .length       = 1,
        .cycles_true  = 4,
        .cycles_false = 4,
        .comment      = "Subtract E+Cy from A",
        .handler      = &opcode_sbc
    },
    {
        .mnemonic     = "SBC A,H",
        .opcode       = 0x9C,
        .length       = 1,
        .cycles_true  = 4,
        .cycles_false = 4,
        .comment      = "Subtract H+Cy from A",
        .handler      = &opcode_sbc
    },
    {
        .mnemonic     = "SBC A,L",
        .opcode       = 0x9D,
        .length       = 1,
        .cycles_true  = 4,
        .cycles_false = 4,
        .comment      = "Subtract L+Cy from A",
        .handler      = &opcode_sbc
    },
    {
        .mnemonic     = "SBC A,(HL)",
        .opcode       = 0x9E,
        .length       = 1,
        .cycles_true  = 8,
        .cycles_false = 8,
        .comment      = "Subtract (HL)+Cy from A",
        .handler      = &opcode_sbc
    },
    {
        .mnemonic     = "SBC A,A",
        .opcode       = 0x9F,
        .length       = 1,
        .cycles_true  = 4,
        .cycles_false = 4,
        .comment      = "Subtract A+Cy from A",
        .handler      = &opcode_sbc
    },
    {
        .mnemonic     = "AND B",
        .opcode       = 0xA0,
        .length       = 1,
        .cycles_true  = 4,
        .cycles_false = 4,
        .comment      = "Logical AND with B",
        .handler      = &opcode_and
    },
    {
        .mnemonic     = "AND C",
        .opcode       = 0xA1,
        .length       = 1,
        .cycles_true  = 4,
        .cycles_false = 4,
        .comment      = "Logical AND with C",
        .handler      = &opcode_and
    },
    {
        .mnemonic     = "AND D",
        .opcode       = 0xA2,
        .length       = 1,
        .cycles_true  = 4,
        .cycles_false = 4,
        .comment      = "Logical AND with D",
        .handler      = &opcode_and
    },
    {
        .mnemonic     = "AND E",
        .opcode       = 0xA3,
        .length       = 1,
        .cycles_true  = 4,
        .cycles_false = 4,
        .comment      = "Logical AND with E",
        .handler      = &opcode_and
    },
    {
        .mnemonic     = "AND H",
        .opcode       = 0xA4,
        .length       = 1,
        .cycles_true  = 4,
        .cycles_false = 4,
        .comment      = "Logical AND with H",
        .handler      = &opcode_and
    },
    {
        .mnemonic     = "AND L",
        .opcode       = 0xA5,
        .length       = 1,
        .cycles_true  = 4,
        .cycles_false = 4,
        .comment      = "Logical AND with L",
        .handler      = &opcode_and
    },
    {
        .mnemonic     = "AND (HL)",
        .opcode       = 0xA6,
        .length       = 1,
        .cycles_true  = 8,
        .cycles_false = 8,
        .comment      = "Logical AND with (HL)",
        .handler      = &opcode_and
    },
    {
        .mnemonic     = "AND A",
        .opcode       = 0xA7,
        .length       = 1,
        .cycles_true  = 4,
        .cycles_false = 4,
        .comment      = "Logical AND with A",
        .handler      = &opcode_and
    },
    {
        .mnemonic     = "XOR B",
        .opcode       = 0xA8,
        .length       = 1,
        .cycles_true  = 4,
        .cycles_false = 4,
        .comment      = "Logical XOR with B",
        .handler      = &opcode_xor
    },
    {
        .mnemonic     = "XOR C",
        .opcode       = 0xA9,
        .length       = 1,
        .cycles_true  = 4,
        .cycles_false = 4,
        .comment      = "Logical XOR with C",
        .handler      = &opcode_xor
    },
    {
        .mnemonic     = "XOR D",
        .opcode       = 0xAA,
        .length       = 1,
        .cycles_true  = 4,
        .cycles_false = 4,
        .comment      = "Logical XOR with D",
        .handler      = &opcode_xor
    },
    {
        .mnemonic     = "XOR E",
        .opcode       = 0xAB,
        .length       = 1,
        .cycles_true  = 4,
        .cycles_false = 4,
        .comment      = "Logical XOR with E",
        .handler      = &opcode_xor
    },
    {
        .mnemonic     = "XOR H",
        .opcode       = 0xAC,
        .length       = 1,
        .cycles_true  = 4,
        .cycles_false = 4,
        .comment      = "Logical XOR with H",
        .handler      = &opcode_xor
    },
    {
        .mnemonic     = "XOR L",
        .opcode       = 0xAD,
        .length       = 1,
        .cycles_true  = 4,
        .cycles_false = 4,
        .comment      = "Logical XOR with L",
        .handler      = &opcode_xor
    },
    {
        .mnemonic     = "XOR (HL)",
        .opcode       = 0xAE,
        .length       = 1,
        .cycles_true  = 8,
        .cycles_false = 8,
        .comment      = "Logical XOR with (HL)",
        .handler      = &opcode_xor
    },
    {
        .mnemonic     = "XOR A",
        .opcode       = 0xAF,
        .length       = 1,
        .cycles_true  = 4,
        .cycles_false = 4,
        .comment      = "Logical XOR with A",
        .handler      = &opcode_xor
    },
    {
        .mnemonic     = "OR B",
        .opcode       = 0xB0,
        .length       = 1,
        .cycles_true  = 4,
        .cycles_false = 4,
        .comment      = "Logical OR with B",
        .handler      = &opcode_or
    },
    {
        .mnemonic     = "OR C",
        .opcode       = 0xB1,
        .length       = 1,
        .cycles_true  = 4,
        .cycles_false = 4,
        .comment      = "Logical OR with C",
        .handler      = &opcode_or
    },
    {
        .mnemonic     = "OR D",
        .opcode       = 0xB2,
        .length       = 1,
        .cycles_true  = 4,
        .cycles_false = 4,
        .comment      = "Logical OR with D",
        .handler      = &opcode_or
    },
    {
        .mnemonic     = "OR E",
        .opcode       = 0xB3,
        .length       = 1,
        .cycles_true  = 4,
        .cycles_false = 4,
        .comment      = "Logical OR with E",
        .handler      = &opcode_or
    },
    {
        .mnemonic     = "OR H",
        .opcode       = 0xB4,
        .length       = 1,
        .cycles_true  = 4,
        .cycles_false = 4,
        .comment      = "Logical OR with H",
        .handler      = &opcode_or
    },
    {
        .mnemonic     = "OR L",
        .opcode       = 0xB5,
        .length       = 1,
        .cycles_true  = 4,
        .cycles_false = 4,
        .comment      = "Logical OR with L",
        .handler      = &opcode_or
    },
    {
        .mnemonic     = "OR (HL)",
        .opcode       = 0xB6,
        .length       = 1,
        .cycles_true  = 8,
        .cycles_false = 8,
        .comment      = "Logical OR with (HL)",
        .handler      = &opcode_or
    },
    {
        .mnemonic     = "OR A",
        .opcode       = 0xB7,
        .length       = 1,
        .cycles_true  = 4,
        .cycles_false = 4,
        .comment      = "Logical OR with A",
        .handler      = &opcode_or
    },
    {
        .mnemonic     = "CP B",
        .opcode       = 0xB8,
        .length       = 1,
        .cycles_true  = 4,
        .cycles_false = 4,
        .comment      = "Compare A with B",
        .handler      = &opcode_cp
    },
    {
        .mnemonic     = "CP C",
        .opcode       = 0xB9,
        .length       = 1,
        .cycles_true  = 4,
        .cycles_false = 4,
        .comment      = "Compare A with C",
        .handler      = &opcode_cp
    },
    {
        .mnemonic     = "CP D",
        .opcode       = 0xBA,
        .length       = 1,
        .cycles_true  = 4,
        .cycles_false = 4,
        .comment      = "Compare A with D",
        .handler      = &opcode_cp
    },
    {
        .mnemonic     = "CP E",
        .opcode       = 0xBB,
        .length       = 1,
        .cycles_true  = 4,
        .cycles_false = 4,
        .comment      = "Compare A with E",
        .handler      = &opcode_cp
    },
    {
        .mnemonic     = "CP H",
        .opcode       = 0xBC,
        .length       = 1,
        .cycles_true  = 4,
        .cycles_false = 4,
        .comment      = "Compare A with H",
        .handler      = &opcode_cp
    },
    {
        .mnemonic     = "CP L",
        .opcode       = 0xBD,
        .length       = 1,
        .cycles_true  = 4,
        .cycles_false = 4,
        .comment      = "Compare A with L",
        .handler      = &opcode_cp
    },
    {
        .mnemonic     = "CP (HL)",
        .opcode       = 0xBE,
        .length       = 1,
        .cycles_true  = 8,
        .cycles_false = 8,
        .comment      = "Compare A with (HL)",
        .handler      = &opcode_cp
    },
    {
        .mnemonic     = "CP A",
        .opcode       = 0xBF,
        .length       = 1,
        .cycles_true  = 4,
        .cycles_false = 4,
        .comment      = "Compare A with A",
        .handler      = &opcode_cp
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
        .mnemonic     = "SUB A,n",
        .opcode       = 0xD6,
        .length       = 2,
        .cycles_true  = 8,
        .cycles_false = 8,
        .comment      = "Subtract n from A",
        .handler      = &opcode_sub
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
        .mnemonic     = "SBC A,n",
        .opcode       = 0xDE,
        .length       = 2,
        .cycles_true  = 8,
        .cycles_false = 8,
        .comment      = "Subtract n+Cy from A",
        .handler      = &opcode_sbc
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
        .mnemonic     = "AND n",
        .opcode       = 0xE6,
        .length       = 2,
        .cycles_true  = 8,
        .cycles_false = 8,
        .comment      = "Logical AND with n",
        .handler      = &opcode_and
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
        .mnemonic     = "ADD SP,n",
        .opcode       = 0xE8,
        .length       = 2,
        .cycles_true  = 16,
        .cycles_false = 16,
        .comment      = "Add signed n to SP",
        .handler      = &opcode_add_sp_n
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
        .mnemonic     = "XOR n",
        .opcode       = 0xEE,
        .length       = 2,
        .cycles_true  = 8,
        .cycles_false = 8,
        .comment      = "Logical XOR with n",
        .handler      = &opcode_xor
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
        .mnemonic     = "OR n",
        .opcode       = 0xF6,
        .length       = 2,
        .cycles_true  = 8,
        .cycles_false = 8,
        .comment      = "Logical OR with n",
        .handler      = &opcode_or
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
        .mnemonic     = "LDHL SP,n",
        .opcode       = 0xF8,
        .length       = 2,
        .cycles_true  = 12,
        .cycles_false = 12,
        .comment      = "Load SP+n (signed) to HL",
        .handler      = &opcode_ld_sp
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
        .mnemonic     = "CP n",
        .opcode       = 0xFE,
        .length       = 2,
        .cycles_true  = 8,
        .cycles_false = 8,
        .comment      = "Compare A with n",
        .handler      = &opcode_cp
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