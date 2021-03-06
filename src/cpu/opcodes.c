/*
opcodes.c
Define opcode_table containing all the CPU's opcodes
Define opcode_cb_table containing all the PREFIX CB opcodes

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
#include "cpu/opcodes/rotate.h"
#include "cpu/opcodes/swap.h"
#include "cpu/opcodes/shifts.h"
#include "cpu/opcodes/bit.h"
#include "cpu/opcodes/res.h"
#include "cpu/opcodes/set.h"
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

const opcode_t opcode_table[256] = {
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
        .handler      = &opcode_ld_bc_nn
    },
    {
        .mnemonic     = "LD (BC),A",
        .opcode       = 0x02,
        .length       = 1,
        .cycles_true  = 8,
        .cycles_false = 8,
        .comment      = "Load A at address BC",
        .handler      = &opcode_ld_bc_a
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
        .handler      = &opcode_ld_b_n
    },
    {
        .mnemonic     = "RLCA",
        .opcode       = 0x07,
        .length       = 1,
        .cycles_true  = 4,
        .cycles_false = 4,
        .comment      = "Rotate A left",
        .handler      = &opcode_rotate_a
    },
    {
        .mnemonic     = "LD (nn),SP",
        .opcode       = 0x08,
        .length       = 3,
        .cycles_true  = 20,
        .cycles_false = 20,
        .comment      = "Load SP at address nn",
        .handler      = &opcode_ld_nn_sp
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
        .handler      = &opcode_ld_a_bc
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
        .handler      = &opcode_ld_c_n
    },
    {
        .mnemonic     = "RRCA",
        .opcode       = 0x0F,
        .length       = 1,
        .cycles_true  = 4,
        .cycles_false = 4,
        .comment      = "Rotate A right",
        .handler      = &opcode_rotate_a
    },
    {
        .mnemonic     = "STOP",
        .opcode       = 0x10,
        .length       = 2,
        .cycles_true  = 4,
        .cycles_false = 4,
        .comment      = "Halt CPU and LCD display until button is pressed",
        .handler      = &opcode_stop
    },
    {
        .mnemonic     = "LD DE,nn",
        .opcode       = 0x11,
        .length       = 3,
        .cycles_true  = 12,
        .cycles_false = 12,
        .comment      = "Load nn to DE",
        .handler      = &opcode_ld_de_nn
    },
    {
        .mnemonic     = "LD (DE),A",
        .opcode       = 0x12,
        .length       = 1,
        .cycles_true  = 8,
        .cycles_false = 8,
        .comment      = "Load A at address DE",
        .handler      = &opcode_ld_de_a
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
        .handler      = &opcode_ld_d_n
    },
    {
        .mnemonic     = "RLA",
        .opcode       = 0x17,
        .length       = 1,
        .cycles_true  = 4,
        .cycles_false = 4,
        .comment      = "Rotate A left through Carry Flag",
        .handler      = &opcode_rotate_a
    },
    {
        .mnemonic     = "JR n",
        .opcode       = 0x18,
        .length       = 2,
        .cycles_true  = 12,
        .cycles_false = 12,
        .comment      = "Jump to PC+n (signed)",
        .handler      = &opcode_jr_n
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
        .handler      = &opcode_ld_a_de
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
        .handler      = &opcode_ld_e_n
    },
    {
        .mnemonic     = "RRA",
        .opcode       = 0x1F,
        .length       = 1,
        .cycles_true  = 4,
        .cycles_false = 4,
        .comment      = "Rotate A right through Carry Flag",
        .handler      = &opcode_rotate_a
    },
    {
        .mnemonic     = "JR NZ,n",
        .opcode       = 0x20,
        .length       = 2,
        .cycles_true  = 12,
        .cycles_false = 8,
        .comment      = "Jump to PC+n (signed) if Z is reset",
        .handler      = &opcode_jr_nz_n
    },
    {
        .mnemonic     = "LD HL,nn",
        .opcode       = 0x21,
        .length       = 3,
        .cycles_true  = 12,
        .cycles_false = 12,
        .comment      = "Load nn to HL",
        .handler      = &opcode_ld_hl_nn
    },
    {
        .mnemonic     = "LDI (HL),A",
        .opcode       = 0x22,
        .length       = 1,
        .cycles_true  = 8,
        .cycles_false = 8,
        .comment      = "Load A at address HL and increment A",
        .handler      = &opcode_ldi_hl_a
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
        .handler      = &opcode_ld_h_n
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
        .handler      = &opcode_jr_z_n
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
        .handler      = &opcode_ldi_a_hl
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
        .handler      = &opcode_ld_l_n
    },
    {
        .mnemonic     = "CPL",
        .opcode       = 0x2F,
        .length       = 1,
        .cycles_true  = 4,
        .cycles_false = 4,
        .comment      = "Complement A (flip all bits)",
        .handler      = &opcode_cpl
    },
    {
        .mnemonic     = "JR NC,n",
        .opcode       = 0x30,
        .length       = 2,
        .cycles_true  = 12,
        .cycles_false = 8,
        .comment      = "Jump to PC+n (signed) if C is reset",
        .handler      = &opcode_jr_nc_n
    },
    {
        .mnemonic     = "LD SP,nn",
        .opcode       = 0x31,
        .length       = 3,
        .cycles_true  = 12,
        .cycles_false = 12,
        .comment      = "Load nn to SP",
        .handler      = &opcode_ld_sp_nn
    },
    {
        .mnemonic     = "LDD (HL),A",
        .opcode       = 0x32,
        .length       = 1,
        .cycles_true  = 8,
        .cycles_false = 8,
        .comment      = "Load A at address HL and decrement A",
        .handler      = &opcode_ldd_hl_a
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
        .handler      = &opcode_ld_hl_n
    },
    {
        .mnemonic     = "SCF",
        .opcode       = 0x37,
        .length       = 1,
        .cycles_true  = 4,
        .cycles_false = 4,
        .comment      = "Set Carry Flag",
        .handler      = &opcode_scf
    },
    {
        .mnemonic     = "JR C,n",
        .opcode       = 0x38,
        .length       = 2,
        .cycles_true  = 12,
        .cycles_false = 8,
        .comment      = "Jump to PC+n (signed) if C is set",
        .handler      = &opcode_jr_c_n
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
        .handler      = &opcode_ldd_a_hl
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
        .handler      = &opcode_ld_a_n
    },
    {
        .mnemonic     = "CCF",
        .opcode       = 0x3F,
        .length       = 1,
        .cycles_true  = 4,
        .cycles_false = 4,
        .comment      = "Complement Carry Flag (flip bit)",
        .handler      = &opcode_ccf
    },
    {
        .mnemonic     = "LD B,B",
        .opcode       = 0x40,
        .length       = 1,
        .cycles_true  = 4,
        .cycles_false = 4,
        .comment      = "Load B to B",
        .handler      = &opcode_ld_b_b
    },
    {
        .mnemonic     = "LD B,C",
        .opcode       = 0x41,
        .length       = 1,
        .cycles_true  = 4,
        .cycles_false = 4,
        .comment      = "Load C to B",
        .handler      = &opcode_ld_b_c
    },
    {
        .mnemonic     = "LD B,D",
        .opcode       = 0x42,
        .length       = 1,
        .cycles_true  = 4,
        .cycles_false = 4,
        .comment      = "Load D to B",
        .handler      = &opcode_ld_b_d
    },
    {
        .mnemonic     = "LD B,E",
        .opcode       = 0x43,
        .length       = 1,
        .cycles_true  = 4,
        .cycles_false = 4,
        .comment      = "Load E to B",
        .handler      = &opcode_ld_b_e
    },
    {
        .mnemonic     = "LD B,H",
        .opcode       = 0x44,
        .length       = 1,
        .cycles_true  = 4,
        .cycles_false = 4,
        .comment      = "Load H to B",
        .handler      = &opcode_ld_b_h
    },
    {
        .mnemonic     = "LD B,L",
        .opcode       = 0x45,
        .length       = 1,
        .cycles_true  = 4,
        .cycles_false = 4,
        .comment      = "Load L to B",
        .handler      = &opcode_ld_b_l
    },
    {
        .mnemonic     = "LD B,(HL)",
        .opcode       = 0x46,
        .length       = 1,
        .cycles_true  = 8,
        .cycles_false = 8,
        .comment      = "Load value at address HL to B",
        .handler      = &opcode_ld_b_hl
    },
    {
        .mnemonic     = "LD B,A",
        .opcode       = 0x47,
        .length       = 1,
        .cycles_true  = 4,
        .cycles_false = 4,
        .comment      = "Load A to B",
        .handler      = &opcode_ld_b_a
    },
    {
        .mnemonic     = "LD C,B",
        .opcode       = 0x48,
        .length       = 1,
        .cycles_true  = 4,
        .cycles_false = 4,
        .comment      = "Load B to C",
        .handler      = &opcode_ld_c_b
    },
    {
        .mnemonic     = "LD C,C",
        .opcode       = 0x49,
        .length       = 1,
        .cycles_true  = 4,
        .cycles_false = 4,
        .comment      = "Load C to C",
        .handler      = &opcode_ld_c_c
    },
    {
        .mnemonic     = "LD C,D",
        .opcode       = 0x4A,
        .length       = 1,
        .cycles_true  = 4,
        .cycles_false = 4,
        .comment      = "Load D to C",
        .handler      = &opcode_ld_c_d
    },
    {
        .mnemonic     = "LD C,E",
        .opcode       = 0x4B,
        .length       = 1,
        .cycles_true  = 4,
        .cycles_false = 4,
        .comment      = "Load E to C",
        .handler      = &opcode_ld_c_e
    },
    {
        .mnemonic     = "LD C,H",
        .opcode       = 0x4C,
        .length       = 1,
        .cycles_true  = 4,
        .cycles_false = 4,
        .comment      = "Load H to C",
        .handler      = &opcode_ld_c_h
    },
    {
        .mnemonic     = "LD C,L",
        .opcode       = 0x4D,
        .length       = 1,
        .cycles_true  = 4,
        .cycles_false = 4,
        .comment      = "Load L to C",
        .handler      = &opcode_ld_c_l
    },
    {
        .mnemonic     = "LD C,(HL)",
        .opcode       = 0x4E,
        .length       = 1,
        .cycles_true  = 8,
        .cycles_false = 8,
        .comment      = "Load value at address HL to C",
        .handler      = &opcode_ld_c_hl
    },
    {
        .mnemonic     = "LD C,A",
        .opcode       = 0x4F,
        .length       = 1,
        .cycles_true  = 4,
        .cycles_false = 4,
        .comment      = "Load A to C",
        .handler      = &opcode_ld_c_a
    },
    {
        .mnemonic     = "LD D,B",
        .opcode       = 0x50,
        .length       = 1,
        .cycles_true  = 4,
        .cycles_false = 4,
        .comment      = "Load B to D",
        .handler      = &opcode_ld_d_b
    },
    {
        .mnemonic     = "LD D,C",
        .opcode       = 0x51,
        .length       = 1,
        .cycles_true  = 4,
        .cycles_false = 4,
        .comment      = "Load C to D",
        .handler      = &opcode_ld_d_c
    },
    {
        .mnemonic     = "LD D,D",
        .opcode       = 0x52,
        .length       = 1,
        .cycles_true  = 4,
        .cycles_false = 4,
        .comment      = "Load D to D",
        .handler      = &opcode_ld_d_d
    },
    {
        .mnemonic     = "LD D,E",
        .opcode       = 0x53,
        .length       = 1,
        .cycles_true  = 4,
        .cycles_false = 4,
        .comment      = "Load E to D",
        .handler      = &opcode_ld_d_e
    },
    {
        .mnemonic     = "LD D,H",
        .opcode       = 0x54,
        .length       = 1,
        .cycles_true  = 4,
        .cycles_false = 4,
        .comment      = "Load H to D",
        .handler      = &opcode_ld_d_h
    },
    {
        .mnemonic     = "LD D,L",
        .opcode       = 0x55,
        .length       = 1,
        .cycles_true  = 4,
        .cycles_false = 4,
        .comment      = "Load L to D",
        .handler      = &opcode_ld_d_l
    },
    {
        .mnemonic     = "LD D,(HL)",
        .opcode       = 0x56,
        .length       = 1,
        .cycles_true  = 8,
        .cycles_false = 8,
        .comment      = "Load value at address HL to D",
        .handler      = &opcode_ld_d_hl
    },
    {
        .mnemonic     = "LD D,A",
        .opcode       = 0x57,
        .length       = 1,
        .cycles_true  = 4,
        .cycles_false = 4,
        .comment      = "Load A to D",
        .handler      = &opcode_ld_d_a
    },
    {
        .mnemonic     = "LD E,B",
        .opcode       = 0x58,
        .length       = 1,
        .cycles_true  = 4,
        .cycles_false = 4,
        .comment      = "Load B to E",
        .handler      = &opcode_ld_e_b
    },
    {
        .mnemonic     = "LD E,C",
        .opcode       = 0x59,
        .length       = 1,
        .cycles_true  = 4,
        .cycles_false = 4,
        .comment      = "Load C to E",
        .handler      = &opcode_ld_e_c
    },
    {
        .mnemonic     = "LD E,D",
        .opcode       = 0x5A,
        .length       = 1,
        .cycles_true  = 4,
        .cycles_false = 4,
        .comment      = "Load D to E",
        .handler      = &opcode_ld_e_d
    },
    {
        .mnemonic     = "LD E,E",
        .opcode       = 0x5B,
        .length       = 1,
        .cycles_true  = 4,
        .cycles_false = 4,
        .comment      = "Load E to E",
        .handler      = &opcode_ld_e_e
    },
    {
        .mnemonic     = "LD E,H",
        .opcode       = 0x5C,
        .length       = 1,
        .cycles_true  = 4,
        .cycles_false = 4,
        .comment      = "Load H to E",
        .handler      = &opcode_ld_e_h
    },
    {
        .mnemonic     = "LD E,L",
        .opcode       = 0x5D,
        .length       = 1,
        .cycles_true  = 4,
        .cycles_false = 4,
        .comment      = "Load L to E",
        .handler      = &opcode_ld_e_l
    },
    {
        .mnemonic     = "LD E,(HL)",
        .opcode       = 0x5E,
        .length       = 1,
        .cycles_true  = 8,
        .cycles_false = 8,
        .comment      = "Load value at address HL to E",
        .handler      = &opcode_ld_e_hl
    },
    {
        .mnemonic     = "LD E,A",
        .opcode       = 0x5F,
        .length       = 1,
        .cycles_true  = 4,
        .cycles_false = 4,
        .comment      = "Load A to E",
        .handler      = &opcode_ld_e_a
    },
    {
        .mnemonic     = "LD H,B",
        .opcode       = 0x60,
        .length       = 1,
        .cycles_true  = 4,
        .cycles_false = 4,
        .comment      = "Load B to H",
        .handler      = &opcode_ld_h_b
    },
    {
        .mnemonic     = "LD H,C",
        .opcode       = 0x61,
        .length       = 1,
        .cycles_true  = 4,
        .cycles_false = 4,
        .comment      = "Load C to H",
        .handler      = &opcode_ld_h_c
    },
    {
        .mnemonic     = "LD H,D",
        .opcode       = 0x62,
        .length       = 1,
        .cycles_true  = 4,
        .cycles_false = 4,
        .comment      = "Load D to H",
        .handler      = &opcode_ld_h_d
    },
    {
        .mnemonic     = "LD H,E",
        .opcode       = 0x63,
        .length       = 1,
        .cycles_true  = 4,
        .cycles_false = 4,
        .comment      = "Load E to H",
        .handler      = &opcode_ld_h_e
    },
    {
        .mnemonic     = "LD H,H",
        .opcode       = 0x64,
        .length       = 1,
        .cycles_true  = 4,
        .cycles_false = 4,
        .comment      = "Load H to H",
        .handler      = &opcode_ld_h_h
    },
    {
        .mnemonic     = "LD H,L",
        .opcode       = 0x65,
        .length       = 1,
        .cycles_true  = 4,
        .cycles_false = 4,
        .comment      = "Load L to H",
        .handler      = &opcode_ld_h_l
    },
    {
        .mnemonic     = "LD H,(HL)",
        .opcode       = 0x66,
        .length       = 1,
        .cycles_true  = 8,
        .cycles_false = 8,
        .comment      = "Load value at address HL to H",
        .handler      = &opcode_ld_h_hl
    },
    {
        .mnemonic     = "LD H,A",
        .opcode       = 0x67,
        .length       = 1,
        .cycles_true  = 4,
        .cycles_false = 4,
        .comment      = "Load A to H",
        .handler      = &opcode_ld_h_a
    },
    {
        .mnemonic     = "LD L,B",
        .opcode       = 0x68,
        .length       = 1,
        .cycles_true  = 4,
        .cycles_false = 4,
        .comment      = "Load B to L",
        .handler      = &opcode_ld_l_b
    },
    {
        .mnemonic     = "LD L,C",
        .opcode       = 0x69,
        .length       = 1,
        .cycles_true  = 4,
        .cycles_false = 4,
        .comment      = "Load C to L",
        .handler      = &opcode_ld_l_c
    },
    {
        .mnemonic     = "LD L,D",
        .opcode       = 0x6A,
        .length       = 1,
        .cycles_true  = 4,
        .cycles_false = 4,
        .comment      = "Load D to L",
        .handler      = &opcode_ld_l_d
    },
    {
        .mnemonic     = "LD L,E",
        .opcode       = 0x6B,
        .length       = 1,
        .cycles_true  = 4,
        .cycles_false = 4,
        .comment      = "Load E to L",
        .handler      = &opcode_ld_l_e
    },
    {
        .mnemonic     = "LD L,H",
        .opcode       = 0x6C,
        .length       = 1,
        .cycles_true  = 4,
        .cycles_false = 4,
        .comment      = "Load H to L",
        .handler      = &opcode_ld_l_h
    },
    {
        .mnemonic     = "LD L,L",
        .opcode       = 0x6D,
        .length       = 1,
        .cycles_true  = 4,
        .cycles_false = 4,
        .comment      = "Load L to L",
        .handler      = &opcode_ld_l_l
    },
    {
        .mnemonic     = "LD L,(HL)",
        .opcode       = 0x6E,
        .length       = 1,
        .cycles_true  = 8,
        .cycles_false = 8,
        .comment      = "Load value at address HL to L",
        .handler      = &opcode_ld_l_hl
    },
    {
        .mnemonic     = "LD L,A",
        .opcode       = 0x6F,
        .length       = 1,
        .cycles_true  = 4,
        .cycles_false = 4,
        .comment      = "Load A to L",
        .handler      = &opcode_ld_l_a
    },
    {
        .mnemonic     = "LD (HL),B",
        .opcode       = 0x70,
        .length       = 1,
        .cycles_true  = 8,
        .cycles_false = 8,
        .comment      = "Load B at address HL",
        .handler      = &opcode_ld_hl_b
    },
    {
        .mnemonic     = "LD (HL),C",
        .opcode       = 0x71,
        .length       = 1,
        .cycles_true  = 8,
        .cycles_false = 8,
        .comment      = "Load C at address HL",
        .handler      = &opcode_ld_hl_c
    },
    {
        .mnemonic     = "LD (HL),D",
        .opcode       = 0x72,
        .length       = 1,
        .cycles_true  = 8,
        .cycles_false = 8,
        .comment      = "Load D at address HL",
        .handler      = &opcode_ld_hl_d
    },
    {
        .mnemonic     = "LD (HL),E",
        .opcode       = 0x73,
        .length       = 1,
        .cycles_true  = 8,
        .cycles_false = 8,
        .comment      = "Load E at address HL",
        .handler      = &opcode_ld_hl_e
    },
    {
        .mnemonic     = "LD (HL),H",
        .opcode       = 0x74,
        .length       = 1,
        .cycles_true  = 8,
        .cycles_false = 8,
        .comment      = "Load H at address HL",
        .handler      = &opcode_ld_hl_h
    },
    {
        .mnemonic     = "LD (HL),L",
        .opcode       = 0x75,
        .length       = 1,
        .cycles_true  = 8,
        .cycles_false = 8,
        .comment      = "Load L at address HL",
        .handler      = &opcode_ld_hl_l
    },
    {
        .mnemonic     = "HALT",
        .opcode       = 0x76,
        .length       = 1,
        .cycles_true  = 4,
        .cycles_false = 4,
        .comment      = "Halt CPU until an interrupt occurs",
        .handler      = &opcode_halt
    },
    {
        .mnemonic     = "LD (HL),A",
        .opcode       = 0x77,
        .length       = 1,
        .cycles_true  = 8,
        .cycles_false = 8,
        .comment      = "Load A at address HL",
        .handler      = &opcode_ld_hl_a
    },
    {
        .mnemonic     = "LD A,B",
        .opcode       = 0x78,
        .length       = 1,
        .cycles_true  = 4,
        .cycles_false = 4,
        .comment      = "Load B to A",
        .handler      = &opcode_ld_a_b
    },
    {
        .mnemonic     = "LD A,C",
        .opcode       = 0x79,
        .length       = 1,
        .cycles_true  = 4,
        .cycles_false = 4,
        .comment      = "Load C to A",
        .handler      = &opcode_ld_a_c
    },
    {
        .mnemonic     = "LD A,D",
        .opcode       = 0x7A,
        .length       = 1,
        .cycles_true  = 4,
        .cycles_false = 4,
        .comment      = "Load D to A",
        .handler      = &opcode_ld_a_d
    },
    {
        .mnemonic     = "LD A,E",
        .opcode       = 0x7B,
        .length       = 1,
        .cycles_true  = 4,
        .cycles_false = 4,
        .comment      = "Load E to A",
        .handler      = &opcode_ld_a_e
    },
    {
        .mnemonic     = "LD A,H",
        .opcode       = 0x7C,
        .length       = 1,
        .cycles_true  = 4,
        .cycles_false = 4,
        .comment      = "Load H to A",
        .handler      = &opcode_ld_a_h
    },
    {
        .mnemonic     = "LD A,L",
        .opcode       = 0x7D,
        .length       = 1,
        .cycles_true  = 4,
        .cycles_false = 4,
        .comment      = "Load L to A",
        .handler      = &opcode_ld_a_l
    },
    {
        .mnemonic     = "LD A,(HL)",
        .opcode       = 0x7E,
        .length       = 1,
        .cycles_true  = 8,
        .cycles_false = 8,
        .comment      = "Load value at address HL to A",
        .handler      = &opcode_ld_a_hl
    },
    {
        .mnemonic     = "LD A,A",
        .opcode       = 0x7F,
        .length       = 1,
        .cycles_true  = 4,
        .cycles_false = 4,
        .comment      = "Load A to A",
        .handler      = &opcode_ld_a_a
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
        .handler      = &opcode_pop_bc
    },
    {
        .mnemonic     = "JP NZ,nn",
        .opcode       = 0xC2,
        .length       = 3,
        .cycles_true  = 16,
        .cycles_false = 12,
        .comment      = "Jump to nn if Z if reset",
        .handler      = &opcode_jp_nz_nn
    },
    {
        .mnemonic     = "JP nn",
        .opcode       = 0xC3,
        .length       = 3,
        .cycles_true  = 16,
        .cycles_false = 16,
        .comment      = "Jump to nn",
        .handler      = &opcode_jp_nn
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
        .handler      = &opcode_push_bc
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
        .handler      = &opcode_jp_z_nn
    },
    {
        .mnemonic     = "PREFIX CB",
        .opcode       = 0xCB,
        .length       = 1,
        .cycles_true  = 4,
        .cycles_false = 4,
        .comment      = "PREFIX CB",
        .handler      = NULL
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
        .handler      = &opcode_pop_de
    },
    {
        .mnemonic     = "JP NC,nn",
        .opcode       = 0xD2,
        .length       = 3,
        .cycles_true  = 16,
        .cycles_false = 12,
        .comment      = "Jump to nn if C if reset",
        .handler      = &opcode_jp_nc_nn
    },
    {NULL, 0, 0, 0, 0, NULL, NULL},
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
        .handler      = &opcode_push_de
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
        .handler      = &opcode_jp_c_nn
    },
    {NULL, 0, 0, 0, 0, NULL, NULL}, // $DB doesn't exist, empty element for alignment
    {
        .mnemonic     = "CALL C,nn",
        .opcode       = 0xDC,
        .length       = 3,
        .cycles_true  = 24,
        .cycles_false = 12,
        .comment      = "Push PC and jump to nn if C if set",
        .handler      = &opcode_call
    },
    {NULL, 0, 0, 0, 0, NULL, NULL}, // $DD doesn't exist, empty element for alignment
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
        .handler      = &opcode_ldh_n_a
    },
    {
        .mnemonic     = "POP HL",
        .opcode       = 0xE1,
        .length       = 1,
        .cycles_true  = 12,
        .cycles_false = 12,
        .comment      = "Pop SP to HL",
        .handler      = &opcode_pop_hl
    },
    {
        .mnemonic     = "LD ($FF00+C),A",
        .opcode       = 0xE2,
        .length       = 2,
        .cycles_true  = 8,
        .cycles_false = 8,
        .comment      = "Load A at address $FF00+C",
        .handler      = &opcode_ld_ff00_c_a
    },
    {NULL, 0, 0, 0, 0, NULL, NULL}, // $E3 doesn't exist, empty element for alignment
    {NULL, 0, 0, 0, 0, NULL, NULL}, // $E4 doesn't exist, empty element for alignment
    {
        .mnemonic     = "PUSH HL",
        .opcode       = 0xE5,
        .length       = 1,
        .cycles_true  = 16,
        .cycles_false = 16,
        .comment      = "Push HL to stack",
        .handler      = &opcode_push_hl
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
        .handler      = &opcode_ld_nn_a
    },
    {NULL, 0, 0, 0, 0, NULL, NULL}, // $EB doesn't exist, empty element for alignment
    {NULL, 0, 0, 0, 0, NULL, NULL}, // $EC doesn't exist, empty element for alignment
    {NULL, 0, 0, 0, 0, NULL, NULL}, // $ED doesn't exist, empty element for alignment
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
        .handler      = &opcode_ldh_a_n
    },
    {
        .mnemonic     = "POP AF",
        .opcode       = 0xF1,
        .length       = 1,
        .cycles_true  = 12,
        .cycles_false = 12,
        .comment      = "Pop SP to AF",
        .handler      = &opcode_pop_af
    },
    {
        .mnemonic     = "LD A,($FF00+C)",
        .opcode       = 0xF2,
        .length       = 2,
        .cycles_true  = 8,
        .cycles_false = 8,
        .comment      = "Load value at address $FF00+C to A",
        .handler      = &opcode_ld_a_ff00_c
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
    {NULL, 0, 0, 0, 0, NULL, NULL}, // $F4 doesn't exist, empty element for alignment
    {
        .mnemonic     = "PUSH AF",
        .opcode       = 0xF5,
        .length       = 1,
        .cycles_true  = 16,
        .cycles_false = 16,
        .comment      = "Push AF to stack",
        .handler      = &opcode_push_af
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
        .mnemonic     = "LDHL SP,e",
        .opcode       = 0xF8,
        .length       = 2,
        .cycles_true  = 12,
        .cycles_false = 12,
        .comment      = "Load SP+e (signed) to HL",
        .handler      = &opcode_ld_sp_e
    },
    {
        .mnemonic     = "LD SP,HL",
        .opcode       = 0xF9,
        .length       = 1,
        .cycles_true  = 8,
        .cycles_false = 8,
        .comment      = "Load HL to SP",
        .handler      = &opcode_ld_sp_hl
    },
    {
        .mnemonic     = "LD A,(nn)",
        .opcode       = 0xFA,
        .length       = 3,
        .cycles_true  = 16,
        .cycles_false = 16,
        .comment      = "Load value at address nn to A",
        .handler      = &opcode_ld_a_nn
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
    {NULL, 0, 0, 0, 0, NULL, NULL}, // $FC doesn't exist, empty element for alignment
    {NULL, 0, 0, 0, 0, NULL, NULL}, // $FD doesn't exist, empty element for alignment
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
    }
};

const opcode_t opcode_cb_table[256] = {
    {
        .mnemonic     = "RLC B",
        .opcode       = 0x00,
        .length       = 2,
        .cycles_true  = 8,
        .cycles_false = 8,
        .comment      = "Rotate B left",
        .handler      = &opcode_cb_rlc_r
    },
    {
        .mnemonic     = "RLC C",
        .opcode       = 0x01,
        .length       = 2,
        .cycles_true  = 8,
        .cycles_false = 8,
        .comment      = "Rotate C left",
        .handler      = &opcode_cb_rlc_r
    },
    {
        .mnemonic     = "RLC D",
        .opcode       = 0x02,
        .length       = 2,
        .cycles_true  = 8,
        .cycles_false = 8,
        .comment      = "Rotate D left",
        .handler      = &opcode_cb_rlc_r
    },
    {
        .mnemonic     = "RLC E",
        .opcode       = 0x03,
        .length       = 2,
        .cycles_true  = 8,
        .cycles_false = 8,
        .comment      = "Rotate E left",
        .handler      = &opcode_cb_rlc_r
    },
    {
        .mnemonic     = "RLC H",
        .opcode       = 0x04,
        .length       = 2,
        .cycles_true  = 8,
        .cycles_false = 8,
        .comment      = "Rotate H left",
        .handler      = &opcode_cb_rlc_r
    },
    {
        .mnemonic     = "RLC L",
        .opcode       = 0x05,
        .length       = 2,
        .cycles_true  = 8,
        .cycles_false = 8,
        .comment      = "Rotate L left",
        .handler      = &opcode_cb_rlc_r
    },
    {
        .mnemonic     = "RLC (HL)",
        .opcode       = 0x06,
        .length       = 2,
        .cycles_true  = 16,
        .cycles_false = 16,
        .comment      = "Rotate (HL) left",
        .handler      = &opcode_cb_rotate_n
    },
    {
        .mnemonic     = "RLC A",
        .opcode       = 0x07,
        .length       = 2,
        .cycles_true  = 8,
        .cycles_false = 8,
        .comment      = "Rotate A left",
        .handler      = &opcode_cb_rlc_r
    },
    {
        .mnemonic     = "RRC B",
        .opcode       = 0x08,
        .length       = 2,
        .cycles_true  = 8,
        .cycles_false = 8,
        .comment      = "Rotate B right",
        .handler      = &opcode_cb_rrc_r
    },
    {
        .mnemonic     = "RRC C",
        .opcode       = 0x09,
        .length       = 2,
        .cycles_true  = 8,
        .cycles_false = 8,
        .comment      = "Rotate C right",
        .handler      = &opcode_cb_rrc_r
    },
    {
        .mnemonic     = "RRC D",
        .opcode       = 0x0A,
        .length       = 2,
        .cycles_true  = 8,
        .cycles_false = 8,
        .comment      = "Rotate D right",
        .handler      = &opcode_cb_rrc_r
    },
    {
        .mnemonic     = "RRC E",
        .opcode       = 0x0B,
        .length       = 2,
        .cycles_true  = 8,
        .cycles_false = 8,
        .comment      = "Rotate E right",
        .handler      = &opcode_cb_rrc_r
    },
    {
        .mnemonic     = "RRC H",
        .opcode       = 0x0C,
        .length       = 2,
        .cycles_true  = 8,
        .cycles_false = 8,
        .comment      = "Rotate H right",
        .handler      = &opcode_cb_rrc_r
    },
    {
        .mnemonic     = "RRC L",
        .opcode       = 0x0D,
        .length       = 2,
        .cycles_true  = 8,
        .cycles_false = 8,
        .comment      = "Rotate L right",
        .handler      = &opcode_cb_rrc_r
    },
    {
        .mnemonic     = "RRC (HL)",
        .opcode       = 0x0E,
        .length       = 2,
        .cycles_true  = 16,
        .cycles_false = 16,
        .comment      = "Rotate (HL) right",
        .handler      = &opcode_cb_rotate_n
    },
    {
        .mnemonic     = "RRC A",
        .opcode       = 0x0F,
        .length       = 2,
        .cycles_true  = 8,
        .cycles_false = 8,
        .comment      = "Rotate A right",
        .handler      = &opcode_cb_rrc_r
    },
    {
        .mnemonic     = "RL B",
        .opcode       = 0x10,
        .length       = 2,
        .cycles_true  = 8,
        .cycles_false = 8,
        .comment      = "Rotate B left through carry",
        .handler      = &opcode_cb_rl_r
    },
    {
        .mnemonic     = "RL C",
        .opcode       = 0x11,
        .length       = 2,
        .cycles_true  = 8,
        .cycles_false = 8,
        .comment      = "Rotate C left through carry",
        .handler      = &opcode_cb_rl_r
    },
    {
        .mnemonic     = "RL D",
        .opcode       = 0x12,
        .length       = 2,
        .cycles_true  = 8,
        .cycles_false = 8,
        .comment      = "Rotate D left through carry",
        .handler      = &opcode_cb_rl_r
    },
    {
        .mnemonic     = "RL E",
        .opcode       = 0x13,
        .length       = 2,
        .cycles_true  = 8,
        .cycles_false = 8,
        .comment      = "Rotate E left through carry",
        .handler      = &opcode_cb_rl_r
    },
    {
        .mnemonic     = "RL H",
        .opcode       = 0x14,
        .length       = 2,
        .cycles_true  = 8,
        .cycles_false = 8,
        .comment      = "Rotate H left through carry",
        .handler      = &opcode_cb_rl_r
    },
    {
        .mnemonic     = "RL L",
        .opcode       = 0x15,
        .length       = 2,
        .cycles_true  = 8,
        .cycles_false = 8,
        .comment      = "Rotate L left through carry",
        .handler      = &opcode_cb_rl_r
    },
    {
        .mnemonic     = "RL (HL)",
        .opcode       = 0x16,
        .length       = 2,
        .cycles_true  = 16,
        .cycles_false = 16,
        .comment      = "Rotate (HL) left through carry",
        .handler      = &opcode_cb_rotate_n
    },
    {
        .mnemonic     = "RL A",
        .opcode       = 0x17,
        .length       = 2,
        .cycles_true  = 8,
        .cycles_false = 8,
        .comment      = "Rotate A left through carry",
        .handler      = &opcode_cb_rl_r
    },
    {
        .mnemonic     = "RR B",
        .opcode       = 0x18,
        .length       = 2,
        .cycles_true  = 8,
        .cycles_false = 8,
        .comment      = "Rotate B right through carry",
        .handler      = &opcode_cb_rr_r
    },
    {
        .mnemonic     = "RR C",
        .opcode       = 0x19,
        .length       = 2,
        .cycles_true  = 8,
        .cycles_false = 8,
        .comment      = "Rotate C right through carry",
        .handler      = &opcode_cb_rr_r
    },
    {
        .mnemonic     = "RR D",
        .opcode       = 0x1A,
        .length       = 2,
        .cycles_true  = 8,
        .cycles_false = 8,
        .comment      = "Rotate D right through carry",
        .handler      = &opcode_cb_rr_r
    },
    {
        .mnemonic     = "RR E",
        .opcode       = 0x1B,
        .length       = 2,
        .cycles_true  = 8,
        .cycles_false = 8,
        .comment      = "Rotate E right through carry",
        .handler      = &opcode_cb_rr_r
    },
    {
        .mnemonic     = "RR H",
        .opcode       = 0x1C,
        .length       = 2,
        .cycles_true  = 8,
        .cycles_false = 8,
        .comment      = "Rotate H right through carry",
        .handler      = &opcode_cb_rr_r
    },
    {
        .mnemonic     = "RR L",
        .opcode       = 0x1D,
        .length       = 2,
        .cycles_true  = 8,
        .cycles_false = 8,
        .comment      = "Rotate L right through carry",
        .handler      = &opcode_cb_rr_r
    },
    {
        .mnemonic     = "RR (HL)",
        .opcode       = 0x1E,
        .length       = 2,
        .cycles_true  = 16,
        .cycles_false = 16,
        .comment      = "Rotate (HL) right through carry",
        .handler      = &opcode_cb_rotate_n
    },
    {
        .mnemonic     = "RR A",
        .opcode       = 0x1F,
        .length       = 2,
        .cycles_true  = 8,
        .cycles_false = 8,
        .comment      = "Rotate A right through carry",
        .handler      = &opcode_cb_rr_r
    },
    {
        .mnemonic     = "SLA B",
        .opcode       = 0x20,
        .length       = 2,
        .cycles_true  = 8,
        .cycles_false = 8,
        .comment      = "Shift B left (LSB=0)",
        .handler      = &opcode_cb_sla_r
    },
    {
        .mnemonic     = "SLA C",
        .opcode       = 0x21,
        .length       = 2,
        .cycles_true  = 8,
        .cycles_false = 8,
        .comment      = "Shift C left (LSB=0)",
        .handler      = &opcode_cb_sla_r
    },
    {
        .mnemonic     = "SLA D",
        .opcode       = 0x22,
        .length       = 2,
        .cycles_true  = 8,
        .cycles_false = 8,
        .comment      = "Shift D left (LSB=0)",
        .handler      = &opcode_cb_sla_r
    },
    {
        .mnemonic     = "SLA E",
        .opcode       = 0x23,
        .length       = 2,
        .cycles_true  = 8,
        .cycles_false = 8,
        .comment      = "Shift E left (LSB=0)",
        .handler      = &opcode_cb_sla_r
    },
    {
        .mnemonic     = "SLA H",
        .opcode       = 0x24,
        .length       = 2,
        .cycles_true  = 8,
        .cycles_false = 8,
        .comment      = "Shift H left (LSB=0)",
        .handler      = &opcode_cb_sla_r
    },
    {
        .mnemonic     = "SLA L",
        .opcode       = 0x25,
        .length       = 2,
        .cycles_true  = 8,
        .cycles_false = 8,
        .comment      = "Shift L left (LSB=0)",
        .handler      = &opcode_cb_sla_r
    },
    {
        .mnemonic     = "SLA (HL)",
        .opcode       = 0x26,
        .length       = 2,
        .cycles_true  = 16,
        .cycles_false = 16,
        .comment      = "Shift (HL) left (LSB=0)",
        .handler      = &opcode_cb_shift_hl
    },
    {
        .mnemonic     = "SLA A",
        .opcode       = 0x27,
        .length       = 2,
        .cycles_true  = 8,
        .cycles_false = 8,
        .comment      = "Shift A left (LSB=0)",
        .handler      = &opcode_cb_sla_r
    },
    {
        .mnemonic     = "SRA B",
        .opcode       = 0x28,
        .length       = 2,
        .cycles_true  = 8,
        .cycles_false = 8,
        .comment      = "Shift B right (MSB doesn't change)",
        .handler      = &opcode_cb_sra_r
    },
    {
        .mnemonic     = "SRA C",
        .opcode       = 0x29,
        .length       = 2,
        .cycles_true  = 8,
        .cycles_false = 8,
        .comment      = "Shift C right (MSB doesn't change)",
        .handler      = &opcode_cb_sra_r
    },
    {
        .mnemonic     = "SRA D",
        .opcode       = 0x2A,
        .length       = 2,
        .cycles_true  = 8,
        .cycles_false = 8,
        .comment      = "Shift D right (MSB doesn't change)",
        .handler      = &opcode_cb_sra_r
    },
    {
        .mnemonic     = "SRA E",
        .opcode       = 0x2B,
        .length       = 2,
        .cycles_true  = 8,
        .cycles_false = 8,
        .comment      = "Shift E right (MSB doesn't change)",
        .handler      = &opcode_cb_sra_r
    },
    {
        .mnemonic     = "SRA H",
        .opcode       = 0x2C,
        .length       = 2,
        .cycles_true  = 8,
        .cycles_false = 8,
        .comment      = "Shift H right (MSB doesn't change)",
        .handler      = &opcode_cb_sra_r
    },
    {
        .mnemonic     = "SRA L",
        .opcode       = 0x2D,
        .length       = 2,
        .cycles_true  = 8,
        .cycles_false = 8,
        .comment      = "Shift L right (MSB doesn't change)",
        .handler      = &opcode_cb_sra_r
    },
    {
        .mnemonic     = "SRA (HL)",
        .opcode       = 0x2E,
        .length       = 2,
        .cycles_true  = 16,
        .cycles_false = 16,
        .comment      = "Shift (HL) right (MSB doesn't change)",
        .handler      = &opcode_cb_shift_hl
    },
    {
        .mnemonic     = "SRA A",
        .opcode       = 0x2F,
        .length       = 2,
        .cycles_true  = 8,
        .cycles_false = 8,
        .comment      = "Shift A right (MSB doesn't change)",
        .handler      = &opcode_cb_sra_r
    },
    {
        .mnemonic     = "SWAP B",
        .opcode       = 0x30,
        .length       = 2,
        .cycles_true  = 8,
        .cycles_false = 8,
        .comment      = "Swap lower and higher 4 bits of B",
        .handler      = &opcode_cb_swap_r
    },
    {
        .mnemonic     = "SWAP C",
        .opcode       = 0x31,
        .length       = 2,
        .cycles_true  = 8,
        .cycles_false = 8,
        .comment      = "Swap lower and higher 4 bits of C",
        .handler      = &opcode_cb_swap_r
    },
    {
        .mnemonic     = "SWAP D",
        .opcode       = 0x32,
        .length       = 2,
        .cycles_true  = 8,
        .cycles_false = 8,
        .comment      = "Swap lower and higher 4 bits of D",
        .handler      = &opcode_cb_swap_r
    },
    {
        .mnemonic     = "SWAP E",
        .opcode       = 0x33,
        .length       = 2,
        .cycles_true  = 8,
        .cycles_false = 8,
        .comment      = "Swap lower and higher 4 bits of E",
        .handler      = &opcode_cb_swap_r
    },
    {
        .mnemonic     = "SWAP H",
        .opcode       = 0x34,
        .length       = 2,
        .cycles_true  = 8,
        .cycles_false = 8,
        .comment      = "Swap lower and higher 4 bits of H",
        .handler      = &opcode_cb_swap_r
    },
    {
        .mnemonic     = "SWAP L",
        .opcode       = 0x35,
        .length       = 2,
        .cycles_true  = 8,
        .cycles_false = 8,
        .comment      = "Swap lower and higher 4 bits of L",
        .handler      = &opcode_cb_swap_r
    },
    {
        .mnemonic     = "SWAP (HL)",
        .opcode       = 0x36,
        .length       = 2,
        .cycles_true  = 16,
        .cycles_false = 16,
        .comment      = "Swap lower and higher 4 bits of (HL)",
        .handler      = &opcode_cb_swap_n
    },
    {
        .mnemonic     = "SWAP A",
        .opcode       = 0x37,
        .length       = 2,
        .cycles_true  = 8,
        .cycles_false = 8,
        .comment      = "Swap lower and higher 4 bits of A",
        .handler      = &opcode_cb_swap_r
    },
    {
        .mnemonic     = "SRL B",
        .opcode       = 0x38,
        .length       = 2,
        .cycles_true  = 8,
        .cycles_false = 8,
        .comment      = "Shift B right",
        .handler      = &opcode_cb_srl_r
    },
    {
        .mnemonic     = "SRL C",
        .opcode       = 0x39,
        .length       = 2,
        .cycles_true  = 8,
        .cycles_false = 8,
        .comment      = "Shift C right (MSB=0)",
        .handler      = &opcode_cb_srl_r
    },
    {
        .mnemonic     = "SRL D",
        .opcode       = 0x3A,
        .length       = 2,
        .cycles_true  = 8,
        .cycles_false = 8,
        .comment      = "Shift D right (MSB=0)",
        .handler      = &opcode_cb_srl_r
    },
    {
        .mnemonic     = "SRL E",
        .opcode       = 0x3B,
        .length       = 2,
        .cycles_true  = 8,
        .cycles_false = 8,
        .comment      = "Shift E right (MSB=0)",
        .handler      = &opcode_cb_srl_r
    },
    {
        .mnemonic     = "SRL H",
        .opcode       = 0x3C,
        .length       = 2,
        .cycles_true  = 8,
        .cycles_false = 8,
        .comment      = "Shift H right (MSB=0)",
        .handler      = &opcode_cb_srl_r
    },
    {
        .mnemonic     = "SRL L",
        .opcode       = 0x3D,
        .length       = 2,
        .cycles_true  = 8,
        .cycles_false = 8,
        .comment      = "Shift L right (MSB=0)",
        .handler      = &opcode_cb_srl_r
    },
    {
        .mnemonic     = "SRL (HL)",
        .opcode       = 0x3E,
        .length       = 2,
        .cycles_true  = 16,
        .cycles_false = 16,
        .comment      = "Shift (HL) right (MSB=0)",
        .handler      = &opcode_cb_shift_hl
    },
    {
        .mnemonic     = "SRL A",
        .opcode       = 0x3F,
        .length       = 2,
        .cycles_true  = 8,
        .cycles_false = 8,
        .comment      = "Shift A right (MSB=0)",
        .handler      = &opcode_cb_srl_r
    },
    {
        .mnemonic     = "BIT 0,B",
        .opcode       = 0x40,
        .length       = 2,
        .cycles_true  = 8,
        .cycles_false = 8,
        .comment      = "Test bit 0 in B",
        .handler      = &opcode_cb_bit
    },
    {
        .mnemonic     = "BIT 0,C",
        .opcode       = 0x41,
        .length       = 2,
        .cycles_true  = 8,
        .cycles_false = 8,
        .comment      = "Test bit 0 in C",
        .handler      = &opcode_cb_bit
    },
    {
        .mnemonic     = "BIT 0,D",
        .opcode       = 0x42,
        .length       = 2,
        .cycles_true  = 8,
        .cycles_false = 8,
        .comment      = "Test bit 0 in D",
        .handler      = &opcode_cb_bit
    },
    {
        .mnemonic     = "BIT 0,E",
        .opcode       = 0x43,
        .length       = 2,
        .cycles_true  = 8,
        .cycles_false = 8,
        .comment      = "Test bit 0 in E",
        .handler      = &opcode_cb_bit
    },
    {
        .mnemonic     = "BIT 0,H",
        .opcode       = 0x44,
        .length       = 2,
        .cycles_true  = 8,
        .cycles_false = 8,
        .comment      = "Test bit 0 in H",
        .handler      = &opcode_cb_bit
    },
    {
        .mnemonic     = "BIT 0,L",
        .opcode       = 0x45,
        .length       = 2,
        .cycles_true  = 8,
        .cycles_false = 8,
        .comment      = "Test bit 0 in L",
        .handler      = &opcode_cb_bit
    },
    {
        .mnemonic     = "BIT 0,(HL)",
        .opcode       = 0x46,
        .length       = 2,
        .cycles_true  = 12,
        .cycles_false = 12,
        .comment      = "Test bit 0 in (HL)",
        .handler      = &opcode_cb_bit
    },
    {
        .mnemonic     = "BIT 0,A",
        .opcode       = 0x47,
        .length       = 2,
        .cycles_true  = 8,
        .cycles_false = 8,
        .comment      = "Test bit 0 in A",
        .handler      = &opcode_cb_bit
    },
    {
        .mnemonic     = "BIT 1,B",
        .opcode       = 0x48,
        .length       = 2,
        .cycles_true  = 8,
        .cycles_false = 8,
        .comment      = "Test bit 1 in B",
        .handler      = &opcode_cb_bit
    },
    {
        .mnemonic     = "BIT 1,C",
        .opcode       = 0x49,
        .length       = 2,
        .cycles_true  = 8,
        .cycles_false = 8,
        .comment      = "Test bit 1 in C",
        .handler      = &opcode_cb_bit
    },
    {
        .mnemonic     = "BIT 1,D",
        .opcode       = 0x4A,
        .length       = 2,
        .cycles_true  = 8,
        .cycles_false = 8,
        .comment      = "Test bit 1 in D",
        .handler      = &opcode_cb_bit
    },
    {
        .mnemonic     = "BIT 1,E",
        .opcode       = 0x4B,
        .length       = 2,
        .cycles_true  = 8,
        .cycles_false = 8,
        .comment      = "Test bit 1 in E",
        .handler      = &opcode_cb_bit
    },
    {
        .mnemonic     = "BIT 1,H",
        .opcode       = 0x4C,
        .length       = 2,
        .cycles_true  = 8,
        .cycles_false = 8,
        .comment      = "Test bit 1 in H",
        .handler      = &opcode_cb_bit
    },
    {
        .mnemonic     = "BIT 1,L",
        .opcode       = 0x4D,
        .length       = 2,
        .cycles_true  = 8,
        .cycles_false = 8,
        .comment      = "Test bit 1 in L",
        .handler      = &opcode_cb_bit
    },
    {
        .mnemonic     = "BIT 1,(HL)",
        .opcode       = 0x4E,
        .length       = 2,
        .cycles_true  = 12,
        .cycles_false = 12,
        .comment      = "Test bit 1 in (HL)",
        .handler      = &opcode_cb_bit
    },
    {
        .mnemonic     = "BIT 1,A",
        .opcode       = 0x4F,
        .length       = 2,
        .cycles_true  = 8,
        .cycles_false = 8,
        .comment      = "Test bit 1 in A",
        .handler      = &opcode_cb_bit
    },
    {
        .mnemonic     = "BIT 2,B",
        .opcode       = 0x50,
        .length       = 2,
        .cycles_true  = 8,
        .cycles_false = 8,
        .comment      = "Test bit 2 in B",
        .handler      = &opcode_cb_bit
    },
    {
        .mnemonic     = "BIT 2,C",
        .opcode       = 0x51,
        .length       = 2,
        .cycles_true  = 8,
        .cycles_false = 8,
        .comment      = "Test bit 2 in C",
        .handler      = &opcode_cb_bit
    },
    {
        .mnemonic     = "BIT 2,D",
        .opcode       = 0x52,
        .length       = 2,
        .cycles_true  = 8,
        .cycles_false = 8,
        .comment      = "Test bit 2 in D",
        .handler      = &opcode_cb_bit
    },
    {
        .mnemonic     = "BIT 2,E",
        .opcode       = 0x53,
        .length       = 2,
        .cycles_true  = 8,
        .cycles_false = 8,
        .comment      = "Test bit 2 in E",
        .handler      = &opcode_cb_bit
    },
    {
        .mnemonic     = "BIT 2,H",
        .opcode       = 0x54,
        .length       = 2,
        .cycles_true  = 8,
        .cycles_false = 8,
        .comment      = "Test bit 2 in H",
        .handler      = &opcode_cb_bit
    },
    {
        .mnemonic     = "BIT 2,L",
        .opcode       = 0x55,
        .length       = 2,
        .cycles_true  = 8,
        .cycles_false = 8,
        .comment      = "Test bit 2 in L",
        .handler      = &opcode_cb_bit
    },
    {
        .mnemonic     = "BIT 2,(HL)",
        .opcode       = 0x56,
        .length       = 2,
        .cycles_true  = 12,
        .cycles_false = 12,
        .comment      = "Test bit 2 in (HL)",
        .handler      = &opcode_cb_bit
    },
    {
        .mnemonic     = "BIT 2,A",
        .opcode       = 0x57,
        .length       = 2,
        .cycles_true  = 8,
        .cycles_false = 8,
        .comment      = "Test bit 2 in A",
        .handler      = &opcode_cb_bit
    },
    {
        .mnemonic     = "BIT 3,B",
        .opcode       = 0x58,
        .length       = 2,
        .cycles_true  = 8,
        .cycles_false = 8,
        .comment      = "Test bit 3 in B",
        .handler      = &opcode_cb_bit
    },
    {
        .mnemonic     = "BIT 3,C",
        .opcode       = 0x59,
        .length       = 2,
        .cycles_true  = 8,
        .cycles_false = 8,
        .comment      = "Test bit 3 in C",
        .handler      = &opcode_cb_bit
    },
    {
        .mnemonic     = "BIT 3,D",
        .opcode       = 0x5A,
        .length       = 2,
        .cycles_true  = 8,
        .cycles_false = 8,
        .comment      = "Test bit 3 in D",
        .handler      = &opcode_cb_bit
    },
    {
        .mnemonic     = "BIT 3,E",
        .opcode       = 0x5B,
        .length       = 2,
        .cycles_true  = 8,
        .cycles_false = 8,
        .comment      = "Test bit 3 in E",
        .handler      = &opcode_cb_bit
    },
    {
        .mnemonic     = "BIT 3,H",
        .opcode       = 0x5C,
        .length       = 2,
        .cycles_true  = 8,
        .cycles_false = 8,
        .comment      = "Test bit 3 in H",
        .handler      = &opcode_cb_bit
    },
    {
        .mnemonic     = "BIT 3,L",
        .opcode       = 0x5D,
        .length       = 2,
        .cycles_true  = 8,
        .cycles_false = 8,
        .comment      = "Test bit 3 in L",
        .handler      = &opcode_cb_bit
    },
    {
        .mnemonic     = "BIT 3,(HL)",
        .opcode       = 0x5E,
        .length       = 2,
        .cycles_true  = 12,
        .cycles_false = 12,
        .comment      = "Test bit 3 in (HL)",
        .handler      = &opcode_cb_bit
    },
    {
        .mnemonic     = "BIT 3,A",
        .opcode       = 0x5F,
        .length       = 2,
        .cycles_true  = 8,
        .cycles_false = 8,
        .comment      = "Test bit 3 in A",
        .handler      = &opcode_cb_bit
    },
    {
        .mnemonic     = "BIT 4,B",
        .opcode       = 0x60,
        .length       = 2,
        .cycles_true  = 8,
        .cycles_false = 8,
        .comment      = "Test bit 4 in B",
        .handler      = &opcode_cb_bit
    },
    {
        .mnemonic     = "BIT 4,C",
        .opcode       = 0x61,
        .length       = 2,
        .cycles_true  = 8,
        .cycles_false = 8,
        .comment      = "Test bit 4 in C",
        .handler      = &opcode_cb_bit
    },
    {
        .mnemonic     = "BIT 4,D",
        .opcode       = 0x62,
        .length       = 2,
        .cycles_true  = 8,
        .cycles_false = 8,
        .comment      = "Test bit 4 in D",
        .handler      = &opcode_cb_bit
    },
    {
        .mnemonic     = "BIT 4,E",
        .opcode       = 0x63,
        .length       = 2,
        .cycles_true  = 8,
        .cycles_false = 8,
        .comment      = "Test bit 4 in E",
        .handler      = &opcode_cb_bit
    },
    {
        .mnemonic     = "BIT 4,H",
        .opcode       = 0x64,
        .length       = 2,
        .cycles_true  = 8,
        .cycles_false = 8,
        .comment      = "Test bit 4 in H",
        .handler      = &opcode_cb_bit
    },
    {
        .mnemonic     = "BIT 4,L",
        .opcode       = 0x65,
        .length       = 2,
        .cycles_true  = 8,
        .cycles_false = 8,
        .comment      = "Test bit 4 in L",
        .handler      = &opcode_cb_bit
    },
    {
        .mnemonic     = "BIT 4,(HL)",
        .opcode       = 0x66,
        .length       = 2,
        .cycles_true  = 12,
        .cycles_false = 12,
        .comment      = "Test bit 4 in (HL)",
        .handler      = &opcode_cb_bit
    },
    {
        .mnemonic     = "BIT 4,A",
        .opcode       = 0x67,
        .length       = 2,
        .cycles_true  = 8,
        .cycles_false = 8,
        .comment      = "Test bit 4 in A",
        .handler      = &opcode_cb_bit
    },
    {
        .mnemonic     = "BIT 5,B",
        .opcode       = 0x68,
        .length       = 2,
        .cycles_true  = 8,
        .cycles_false = 8,
        .comment      = "Test bit 5 in B",
        .handler      = &opcode_cb_bit
    },
    {
        .mnemonic     = "BIT 5,C",
        .opcode       = 0x69,
        .length       = 2,
        .cycles_true  = 8,
        .cycles_false = 8,
        .comment      = "Test bit 5 in C",
        .handler      = &opcode_cb_bit
    },
    {
        .mnemonic     = "BIT 5,D",
        .opcode       = 0x6A,
        .length       = 2,
        .cycles_true  = 8,
        .cycles_false = 8,
        .comment      = "Test bit 5 in D",
        .handler      = &opcode_cb_bit
    },
    {
        .mnemonic     = "BIT 5,E",
        .opcode       = 0x6B,
        .length       = 2,
        .cycles_true  = 8,
        .cycles_false = 8,
        .comment      = "Test bit 5 in E",
        .handler      = &opcode_cb_bit
    },
    {
        .mnemonic     = "BIT 5,H",
        .opcode       = 0x6C,
        .length       = 2,
        .cycles_true  = 8,
        .cycles_false = 8,
        .comment      = "Test bit 5 in H",
        .handler      = &opcode_cb_bit
    },
    {
        .mnemonic     = "BIT 5,L",
        .opcode       = 0x6D,
        .length       = 2,
        .cycles_true  = 8,
        .cycles_false = 8,
        .comment      = "Test bit 5 in L",
        .handler      = &opcode_cb_bit
    },
    {
        .mnemonic     = "BIT 5,(HL)",
        .opcode       = 0x6E,
        .length       = 2,
        .cycles_true  = 12,
        .cycles_false = 12,
        .comment      = "Test bit 5 in (HL)",
        .handler      = &opcode_cb_bit
    },
    {
        .mnemonic     = "BIT 5,A",
        .opcode       = 0x6F,
        .length       = 2,
        .cycles_true  = 8,
        .cycles_false = 8,
        .comment      = "Test bit 5 in A",
        .handler      = &opcode_cb_bit
    },
    {
        .mnemonic     = "BIT 6,B",
        .opcode       = 0x70,
        .length       = 2,
        .cycles_true  = 8,
        .cycles_false = 8,
        .comment      = "Test bit 6 in B",
        .handler      = &opcode_cb_bit
    },
    {
        .mnemonic     = "BIT 6,C",
        .opcode       = 0x71,
        .length       = 2,
        .cycles_true  = 8,
        .cycles_false = 8,
        .comment      = "Test bit 6 in C",
        .handler      = &opcode_cb_bit
    },
    {
        .mnemonic     = "BIT 6,D",
        .opcode       = 0x72,
        .length       = 2,
        .cycles_true  = 8,
        .cycles_false = 8,
        .comment      = "Test bit 6 in D",
        .handler      = &opcode_cb_bit
    },
    {
        .mnemonic     = "BIT 6,E",
        .opcode       = 0x73,
        .length       = 2,
        .cycles_true  = 8,
        .cycles_false = 8,
        .comment      = "Test bit 6 in E",
        .handler      = &opcode_cb_bit
    },
    {
        .mnemonic     = "BIT 6,H",
        .opcode       = 0x74,
        .length       = 2,
        .cycles_true  = 8,
        .cycles_false = 8,
        .comment      = "Test bit 6 in H",
        .handler      = &opcode_cb_bit
    },
    {
        .mnemonic     = "BIT 6,L",
        .opcode       = 0x75,
        .length       = 2,
        .cycles_true  = 8,
        .cycles_false = 8,
        .comment      = "Test bit 6 in L",
        .handler      = &opcode_cb_bit
    },
    {
        .mnemonic     = "BIT 6,(HL)",
        .opcode       = 0x76,
        .length       = 2,
        .cycles_true  = 12,
        .cycles_false = 12,
        .comment      = "Test bit 6 in (HL)",
        .handler      = &opcode_cb_bit
    },
    {
        .mnemonic     = "BIT 6,A",
        .opcode       = 0x77,
        .length       = 2,
        .cycles_true  = 8,
        .cycles_false = 8,
        .comment      = "Test bit 6 in A",
        .handler      = &opcode_cb_bit
    },
    {
        .mnemonic     = "BIT 7,B",
        .opcode       = 0x78,
        .length       = 2,
        .cycles_true  = 8,
        .cycles_false = 8,
        .comment      = "Test bit 7 in B",
        .handler      = &opcode_cb_bit
    },
    {
        .mnemonic     = "BIT 7,C",
        .opcode       = 0x79,
        .length       = 2,
        .cycles_true  = 8,
        .cycles_false = 8,
        .comment      = "Test bit 7 in C",
        .handler      = &opcode_cb_bit
    },
    {
        .mnemonic     = "BIT 7,D",
        .opcode       = 0x7A,
        .length       = 2,
        .cycles_true  = 8,
        .cycles_false = 8,
        .comment      = "Test bit 7 in D",
        .handler      = &opcode_cb_bit
    },
    {
        .mnemonic     = "BIT 7,E",
        .opcode       = 0x7B,
        .length       = 2,
        .cycles_true  = 8,
        .cycles_false = 8,
        .comment      = "Test bit 7 in E",
        .handler      = &opcode_cb_bit
    },
    {
        .mnemonic     = "BIT 7,H",
        .opcode       = 0x7C,
        .length       = 2,
        .cycles_true  = 8,
        .cycles_false = 8,
        .comment      = "Test bit 7 in H",
        .handler      = &opcode_cb_bit
    },
    {
        .mnemonic     = "BIT 7,L",
        .opcode       = 0x7D,
        .length       = 2,
        .cycles_true  = 8,
        .cycles_false = 8,
        .comment      = "Test bit 7 in L",
        .handler      = &opcode_cb_bit
    },
    {
        .mnemonic     = "BIT 7,(HL)",
        .opcode       = 0x7E,
        .length       = 2,
        .cycles_true  = 12,
        .cycles_false = 12,
        .comment      = "Test bit 7 in (HL)",
        .handler      = &opcode_cb_bit
    },
    {
        .mnemonic     = "BIT 7,A",
        .opcode       = 0x7F,
        .length       = 2,
        .cycles_true  = 8,
        .cycles_false = 8,
        .comment      = "Test bit 7 in A",
        .handler      = &opcode_cb_bit
    },
    {
        .mnemonic     = "RES 0,B",
        .opcode       = 0x80,
        .length       = 2,
        .cycles_true  = 8,
        .cycles_false = 8,
        .comment      = "Reset bit 0 in B",
        .handler      = &opcode_cb_res
    },
    {
        .mnemonic     = "RES 0,C",
        .opcode       = 0x81,
        .length       = 2,
        .cycles_true  = 8,
        .cycles_false = 8,
        .comment      = "Reset bit 0 in C",
        .handler      = &opcode_cb_res
    },
    {
        .mnemonic     = "RES 0,D",
        .opcode       = 0x82,
        .length       = 2,
        .cycles_true  = 8,
        .cycles_false = 8,
        .comment      = "Reset bit 0 in D",
        .handler      = &opcode_cb_res
    },
    {
        .mnemonic     = "RES 0,E",
        .opcode       = 0x83,
        .length       = 2,
        .cycles_true  = 8,
        .cycles_false = 8,
        .comment      = "Reset bit 0 in E",
        .handler      = &opcode_cb_res
    },
    {
        .mnemonic     = "RES 0,H",
        .opcode       = 0x84,
        .length       = 2,
        .cycles_true  = 8,
        .cycles_false = 8,
        .comment      = "Reset bit 0 in H",
        .handler      = &opcode_cb_res
    },
    {
        .mnemonic     = "RES 0,L",
        .opcode       = 0x85,
        .length       = 2,
        .cycles_true  = 8,
        .cycles_false = 8,
        .comment      = "Reset bit 0 in L",
        .handler      = &opcode_cb_res
    },
    {
        .mnemonic     = "RES 0,(HL)",
        .opcode       = 0x86,
        .length       = 2,
        .cycles_true  = 16,
        .cycles_false = 16,
        .comment      = "Reset bit 0 in (HL)",
        .handler      = &opcode_cb_res
    },
    {
        .mnemonic     = "RES 0,A",
        .opcode       = 0x87,
        .length       = 2,
        .cycles_true  = 8,
        .cycles_false = 8,
        .comment      = "Reset bit 0 in A",
        .handler      = &opcode_cb_res
    },
    {
        .mnemonic     = "RES 1,B",
        .opcode       = 0x88,
        .length       = 2,
        .cycles_true  = 8,
        .cycles_false = 8,
        .comment      = "Reset bit 1 in B",
        .handler      = &opcode_cb_res
    },
    {
        .mnemonic     = "RES 1,C",
        .opcode       = 0x89,
        .length       = 2,
        .cycles_true  = 8,
        .cycles_false = 8,
        .comment      = "Reset bit 1 in C",
        .handler      = &opcode_cb_res
    },
    {
        .mnemonic     = "RES 1,D",
        .opcode       = 0x8A,
        .length       = 2,
        .cycles_true  = 8,
        .cycles_false = 8,
        .comment      = "Reset bit 1 in D",
        .handler      = &opcode_cb_res
    },
    {
        .mnemonic     = "RES 1,E",
        .opcode       = 0x8B,
        .length       = 2,
        .cycles_true  = 8,
        .cycles_false = 8,
        .comment      = "Reset bit 1 in E",
        .handler      = &opcode_cb_res
    },
    {
        .mnemonic     = "RES 1,H",
        .opcode       = 0x8C,
        .length       = 2,
        .cycles_true  = 8,
        .cycles_false = 8,
        .comment      = "Reset bit 1 in H",
        .handler      = &opcode_cb_res
    },
    {
        .mnemonic     = "RES 1,L",
        .opcode       = 0x8D,
        .length       = 2,
        .cycles_true  = 8,
        .cycles_false = 8,
        .comment      = "Reset bit 1 in L",
        .handler      = &opcode_cb_res
    },
    {
        .mnemonic     = "RES 1,(HL)",
        .opcode       = 0x8E,
        .length       = 2,
        .cycles_true  = 16,
        .cycles_false = 16,
        .comment      = "Reset bit 1 in (HL)",
        .handler      = &opcode_cb_res
    },
    {
        .mnemonic     = "RES 1,A",
        .opcode       = 0x8F,
        .length       = 2,
        .cycles_true  = 8,
        .cycles_false = 8,
        .comment      = "Reset bit 1 in A",
        .handler      = &opcode_cb_res
    },
    {
        .mnemonic     = "RES 2,B",
        .opcode       = 0x90,
        .length       = 2,
        .cycles_true  = 8,
        .cycles_false = 8,
        .comment      = "Reset bit 2 in B",
        .handler      = &opcode_cb_res
    },
    {
        .mnemonic     = "RES 2,C",
        .opcode       = 0x91,
        .length       = 2,
        .cycles_true  = 8,
        .cycles_false = 8,
        .comment      = "Reset bit 2 in C",
        .handler      = &opcode_cb_res
    },
    {
        .mnemonic     = "RES 2,D",
        .opcode       = 0x92,
        .length       = 2,
        .cycles_true  = 8,
        .cycles_false = 8,
        .comment      = "Reset bit 2 in D",
        .handler      = &opcode_cb_res
    },
    {
        .mnemonic     = "RES 2,E",
        .opcode       = 0x93,
        .length       = 2,
        .cycles_true  = 8,
        .cycles_false = 8,
        .comment      = "Reset bit 2 in E",
        .handler      = &opcode_cb_res
    },
    {
        .mnemonic     = "RES 2,H",
        .opcode       = 0x94,
        .length       = 2,
        .cycles_true  = 8,
        .cycles_false = 8,
        .comment      = "Reset bit 2 in H",
        .handler      = &opcode_cb_res
    },
    {
        .mnemonic     = "RES 2,L",
        .opcode       = 0x95,
        .length       = 2,
        .cycles_true  = 8,
        .cycles_false = 8,
        .comment      = "Reset bit 2 in L",
        .handler      = &opcode_cb_res
    },
    {
        .mnemonic     = "RES 2,(HL)",
        .opcode       = 0x96,
        .length       = 2,
        .cycles_true  = 16,
        .cycles_false = 16,
        .comment      = "Reset bit 2 in (HL)",
        .handler      = &opcode_cb_res
    },
    {
        .mnemonic     = "RES 2,A",
        .opcode       = 0x97,
        .length       = 2,
        .cycles_true  = 8,
        .cycles_false = 8,
        .comment      = "Reset bit 2 in A",
        .handler      = &opcode_cb_res
    },
    {
        .mnemonic     = "RES 3,B",
        .opcode       = 0x98,
        .length       = 2,
        .cycles_true  = 8,
        .cycles_false = 8,
        .comment      = "Reset bit 3 in B",
        .handler      = &opcode_cb_res
    },
    {
        .mnemonic     = "RES 3,C",
        .opcode       = 0x99,
        .length       = 2,
        .cycles_true  = 8,
        .cycles_false = 8,
        .comment      = "Reset bit 3 in C",
        .handler      = &opcode_cb_res
    },
    {
        .mnemonic     = "RES 3,D",
        .opcode       = 0x9A,
        .length       = 2,
        .cycles_true  = 8,
        .cycles_false = 8,
        .comment      = "Reset bit 3 in D",
        .handler      = &opcode_cb_res
    },
    {
        .mnemonic     = "RES 3,E",
        .opcode       = 0x9B,
        .length       = 2,
        .cycles_true  = 8,
        .cycles_false = 8,
        .comment      = "Reset bit 3 in E",
        .handler      = &opcode_cb_res
    },
    {
        .mnemonic     = "RES 3,H",
        .opcode       = 0x9C,
        .length       = 2,
        .cycles_true  = 8,
        .cycles_false = 8,
        .comment      = "Reset bit 3 in H",
        .handler      = &opcode_cb_res
    },
    {
        .mnemonic     = "RES 3,L",
        .opcode       = 0x9D,
        .length       = 2,
        .cycles_true  = 8,
        .cycles_false = 8,
        .comment      = "Reset bit 3 in L",
        .handler      = &opcode_cb_res
    },
    {
        .mnemonic     = "RES 3,(HL)",
        .opcode       = 0x9E,
        .length       = 2,
        .cycles_true  = 16,
        .cycles_false = 16,
        .comment      = "Reset bit 3 in (HL)",
        .handler      = &opcode_cb_res
    },
    {
        .mnemonic     = "RES 3,A",
        .opcode       = 0x9F,
        .length       = 2,
        .cycles_true  = 8,
        .cycles_false = 8,
        .comment      = "Reset bit 3 in A",
        .handler      = &opcode_cb_res
    },
    {
        .mnemonic     = "RES 4,B",
        .opcode       = 0xA0,
        .length       = 2,
        .cycles_true  = 8,
        .cycles_false = 8,
        .comment      = "Reset bit 4 in B",
        .handler      = &opcode_cb_res
    },
    {
        .mnemonic     = "RES 4,C",
        .opcode       = 0xA1,
        .length       = 2,
        .cycles_true  = 8,
        .cycles_false = 8,
        .comment      = "Reset bit 4 in C",
        .handler      = &opcode_cb_res
    },
    {
        .mnemonic     = "RES 4,D",
        .opcode       = 0xA2,
        .length       = 2,
        .cycles_true  = 8,
        .cycles_false = 8,
        .comment      = "Reset bit 4 in D",
        .handler      = &opcode_cb_res
    },
    {
        .mnemonic     = "RES 4,E",
        .opcode       = 0xA3,
        .length       = 2,
        .cycles_true  = 8,
        .cycles_false = 8,
        .comment      = "Reset bit 4 in E",
        .handler      = &opcode_cb_res
    },
    {
        .mnemonic     = "RES 4,H",
        .opcode       = 0xA4,
        .length       = 2,
        .cycles_true  = 8,
        .cycles_false = 8,
        .comment      = "Reset bit 4 in H",
        .handler      = &opcode_cb_res
    },
    {
        .mnemonic     = "RES 4,L",
        .opcode       = 0xA5,
        .length       = 2,
        .cycles_true  = 8,
        .cycles_false = 8,
        .comment      = "Reset bit 4 in L",
        .handler      = &opcode_cb_res
    },
    {
        .mnemonic     = "RES 4,(HL)",
        .opcode       = 0xA6,
        .length       = 2,
        .cycles_true  = 16,
        .cycles_false = 16,
        .comment      = "Reset bit 4 in (HL)",
        .handler      = &opcode_cb_res
    },
    {
        .mnemonic     = "RES 4,A",
        .opcode       = 0xA7,
        .length       = 2,
        .cycles_true  = 8,
        .cycles_false = 8,
        .comment      = "Reset bit 4 in A",
        .handler      = &opcode_cb_res
    },
    {
        .mnemonic     = "RES 5,B",
        .opcode       = 0xA8,
        .length       = 2,
        .cycles_true  = 8,
        .cycles_false = 8,
        .comment      = "Reset bit 5 in B",
        .handler      = &opcode_cb_res
    },
    {
        .mnemonic     = "RES 5,C",
        .opcode       = 0xA9,
        .length       = 2,
        .cycles_true  = 8,
        .cycles_false = 8,
        .comment      = "Reset bit 5 in C",
        .handler      = &opcode_cb_res
    },
    {
        .mnemonic     = "RES 5,D",
        .opcode       = 0xAA,
        .length       = 2,
        .cycles_true  = 8,
        .cycles_false = 8,
        .comment      = "Reset bit 5 in D",
        .handler      = &opcode_cb_res
    },
    {
        .mnemonic     = "RES 5,E",
        .opcode       = 0xAB,
        .length       = 2,
        .cycles_true  = 8,
        .cycles_false = 8,
        .comment      = "Reset bit 5 in E",
        .handler      = &opcode_cb_res
    },
    {
        .mnemonic     = "RES 5,H",
        .opcode       = 0xAC,
        .length       = 2,
        .cycles_true  = 8,
        .cycles_false = 8,
        .comment      = "Reset bit 5 in H",
        .handler      = &opcode_cb_res
    },
    {
        .mnemonic     = "RES 5,L",
        .opcode       = 0xAD,
        .length       = 2,
        .cycles_true  = 8,
        .cycles_false = 8,
        .comment      = "Reset bit 5 in L",
        .handler      = &opcode_cb_res
    },
    {
        .mnemonic     = "RES 5,(HL)",
        .opcode       = 0xAE,
        .length       = 2,
        .cycles_true  = 16,
        .cycles_false = 16,
        .comment      = "Reset bit 5 in (HL)",
        .handler      = &opcode_cb_res
    },
    {
        .mnemonic     = "RES 5,A",
        .opcode       = 0xAF,
        .length       = 2,
        .cycles_true  = 8,
        .cycles_false = 8,
        .comment      = "Reset bit 5 in A",
        .handler      = &opcode_cb_res
    },
    {
        .mnemonic     = "RES 6,B",
        .opcode       = 0xB0,
        .length       = 2,
        .cycles_true  = 8,
        .cycles_false = 8,
        .comment      = "Reset bit 6 in B",
        .handler      = &opcode_cb_res
    },
    {
        .mnemonic     = "RES 6,C",
        .opcode       = 0xB1,
        .length       = 2,
        .cycles_true  = 8,
        .cycles_false = 8,
        .comment      = "Reset bit 6 in C",
        .handler      = &opcode_cb_res
    },
    {
        .mnemonic     = "RES 6,D",
        .opcode       = 0xB2,
        .length       = 2,
        .cycles_true  = 8,
        .cycles_false = 8,
        .comment      = "Reset bit 6 in D",
        .handler      = &opcode_cb_res
    },
    {
        .mnemonic     = "RES 6,E",
        .opcode       = 0xB3,
        .length       = 2,
        .cycles_true  = 8,
        .cycles_false = 8,
        .comment      = "Reset bit 6 in E",
        .handler      = &opcode_cb_res
    },
    {
        .mnemonic     = "RES 6,H",
        .opcode       = 0xB4,
        .length       = 2,
        .cycles_true  = 8,
        .cycles_false = 8,
        .comment      = "Reset bit 6 in H",
        .handler      = &opcode_cb_res
    },
    {
        .mnemonic     = "RES 6,L",
        .opcode       = 0xB5,
        .length       = 2,
        .cycles_true  = 8,
        .cycles_false = 8,
        .comment      = "Reset bit 6 in L",
        .handler      = &opcode_cb_res
    },
    {
        .mnemonic     = "RES 6,(HL)",
        .opcode       = 0xB6,
        .length       = 2,
        .cycles_true  = 16,
        .cycles_false = 16,
        .comment      = "Reset bit 6 in (HL)",
        .handler      = &opcode_cb_res
    },
    {
        .mnemonic     = "RES 6,A",
        .opcode       = 0xB7,
        .length       = 2,
        .cycles_true  = 8,
        .cycles_false = 8,
        .comment      = "Reset bit 6 in A",
        .handler      = &opcode_cb_res
    },
    {
        .mnemonic     = "RES 7,B",
        .opcode       = 0xB8,
        .length       = 2,
        .cycles_true  = 8,
        .cycles_false = 8,
        .comment      = "Reset bit 7 in B",
        .handler      = &opcode_cb_res
    },
    {
        .mnemonic     = "RES 7,C",
        .opcode       = 0xB9,
        .length       = 2,
        .cycles_true  = 8,
        .cycles_false = 8,
        .comment      = "Reset bit 7 in C",
        .handler      = &opcode_cb_res
    },
    {
        .mnemonic     = "RES 7,D",
        .opcode       = 0xBA,
        .length       = 2,
        .cycles_true  = 8,
        .cycles_false = 8,
        .comment      = "Reset bit 7 in D",
        .handler      = &opcode_cb_res
    },
    {
        .mnemonic     = "RES 7,E",
        .opcode       = 0xBB,
        .length       = 2,
        .cycles_true  = 8,
        .cycles_false = 8,
        .comment      = "Reset bit 7 in E",
        .handler      = &opcode_cb_res
    },
    {
        .mnemonic     = "RES 7,H",
        .opcode       = 0xBC,
        .length       = 2,
        .cycles_true  = 8,
        .cycles_false = 8,
        .comment      = "Reset bit 7 in H",
        .handler      = &opcode_cb_res
    },
    {
        .mnemonic     = "RES 7,L",
        .opcode       = 0xBD,
        .length       = 2,
        .cycles_true  = 8,
        .cycles_false = 8,
        .comment      = "Reset bit 7 in L",
        .handler      = &opcode_cb_res
    },
    {
        .mnemonic     = "RES 7,(HL)",
        .opcode       = 0xBE,
        .length       = 2,
        .cycles_true  = 16,
        .cycles_false = 16,
        .comment      = "Reset bit 7 in (HL)",
        .handler      = &opcode_cb_res
    },
    {
        .mnemonic     = "RES 7,A",
        .opcode       = 0xBF,
        .length       = 2,
        .cycles_true  = 8,
        .cycles_false = 8,
        .comment      = "Reset bit 7 in A",
        .handler      = &opcode_cb_res
    },
    {
        .mnemonic     = "SET 0,B",
        .opcode       = 0xC0,
        .length       = 2,
        .cycles_true  = 8,
        .cycles_false = 8,
        .comment      = "Set bit 0 in B",
        .handler      = &opcode_cb_set
    },
    {
        .mnemonic     = "SET 0,C",
        .opcode       = 0xC1,
        .length       = 2,
        .cycles_true  = 8,
        .cycles_false = 8,
        .comment      = "Set bit 0 in C",
        .handler      = &opcode_cb_set
    },
    {
        .mnemonic     = "SET 0,D",
        .opcode       = 0xC2,
        .length       = 2,
        .cycles_true  = 8,
        .cycles_false = 8,
        .comment      = "Set bit 0 in D",
        .handler      = &opcode_cb_set
    },
    {
        .mnemonic     = "SET 0,E",
        .opcode       = 0xC3,
        .length       = 2,
        .cycles_true  = 8,
        .cycles_false = 8,
        .comment      = "Set bit 0 in E",
        .handler      = &opcode_cb_set
    },
    {
        .mnemonic     = "SET 0,H",
        .opcode       = 0xC4,
        .length       = 2,
        .cycles_true  = 8,
        .cycles_false = 8,
        .comment      = "Set bit 0 in H",
        .handler      = &opcode_cb_set
    },
    {
        .mnemonic     = "SET 0,L",
        .opcode       = 0xC5,
        .length       = 2,
        .cycles_true  = 8,
        .cycles_false = 8,
        .comment      = "Set bit 0 in L",
        .handler      = &opcode_cb_set
    },
    {
        .mnemonic     = "SET 0,(HL)",
        .opcode       = 0xC6,
        .length       = 2,
        .cycles_true  = 16,
        .cycles_false = 16,
        .comment      = "Set bit 0 in (HL)",
        .handler      = &opcode_cb_set
    },
    {
        .mnemonic     = "SET 0,A",
        .opcode       = 0xC7,
        .length       = 2,
        .cycles_true  = 8,
        .cycles_false = 8,
        .comment      = "Set bit 0 in A",
        .handler      = &opcode_cb_set
    },
    {
        .mnemonic     = "SET 1,B",
        .opcode       = 0xC8,
        .length       = 2,
        .cycles_true  = 8,
        .cycles_false = 8,
        .comment      = "Set bit 1 in B",
        .handler      = &opcode_cb_set
    },
    {
        .mnemonic     = "SET 1,C",
        .opcode       = 0xC9,
        .length       = 2,
        .cycles_true  = 8,
        .cycles_false = 8,
        .comment      = "Set bit 1 in C",
        .handler      = &opcode_cb_set
    },
    {
        .mnemonic     = "SET 1,D",
        .opcode       = 0xCA,
        .length       = 2,
        .cycles_true  = 8,
        .cycles_false = 8,
        .comment      = "Set bit 1 in D",
        .handler      = &opcode_cb_set
    },
    {
        .mnemonic     = "SET 1,E",
        .opcode       = 0xCB,
        .length       = 2,
        .cycles_true  = 8,
        .cycles_false = 8,
        .comment      = "Set bit 1 in E",
        .handler      = &opcode_cb_set
    },
    {
        .mnemonic     = "SET 1,H",
        .opcode       = 0xCC,
        .length       = 2,
        .cycles_true  = 8,
        .cycles_false = 8,
        .comment      = "Set bit 1 in H",
        .handler      = &opcode_cb_set
    },
    {
        .mnemonic     = "SET 1,L",
        .opcode       = 0xCD,
        .length       = 2,
        .cycles_true  = 8,
        .cycles_false = 8,
        .comment      = "Set bit 1 in L",
        .handler      = &opcode_cb_set
    },
    {
        .mnemonic     = "SET 1,(HL)",
        .opcode       = 0xCE,
        .length       = 2,
        .cycles_true  = 16,
        .cycles_false = 16,
        .comment      = "Set bit 1 in (HL)",
        .handler      = &opcode_cb_set
    },
    {
        .mnemonic     = "SET 1,A",
        .opcode       = 0xCF,
        .length       = 2,
        .cycles_true  = 8,
        .cycles_false = 8,
        .comment      = "Set bit 1 in A",
        .handler      = &opcode_cb_set
    },
    {
        .mnemonic     = "SET 2,B",
        .opcode       = 0xD0,
        .length       = 2,
        .cycles_true  = 8,
        .cycles_false = 8,
        .comment      = "Set bit 2 in B",
        .handler      = &opcode_cb_set
    },
    {
        .mnemonic     = "SET 2,C",
        .opcode       = 0xD1,
        .length       = 2,
        .cycles_true  = 8,
        .cycles_false = 8,
        .comment      = "Set bit 2 in C",
        .handler      = &opcode_cb_set
    },
    {
        .mnemonic     = "SET 2,D",
        .opcode       = 0xD2,
        .length       = 2,
        .cycles_true  = 8,
        .cycles_false = 8,
        .comment      = "Set bit 2 in D",
        .handler      = &opcode_cb_set
    },
    {
        .mnemonic     = "SET 2,E",
        .opcode       = 0xD3,
        .length       = 2,
        .cycles_true  = 8,
        .cycles_false = 8,
        .comment      = "Set bit 2 in E",
        .handler      = &opcode_cb_set
    },
    {
        .mnemonic     = "SET 2,H",
        .opcode       = 0xD4,
        .length       = 2,
        .cycles_true  = 8,
        .cycles_false = 8,
        .comment      = "Set bit 2 in H",
        .handler      = &opcode_cb_set
    },
    {
        .mnemonic     = "SET 2,L",
        .opcode       = 0xD5,
        .length       = 2,
        .cycles_true  = 8,
        .cycles_false = 8,
        .comment      = "Set bit 2 in L",
        .handler      = &opcode_cb_set
    },
    {
        .mnemonic     = "SET 2,(HL)",
        .opcode       = 0xD6,
        .length       = 2,
        .cycles_true  = 16,
        .cycles_false = 16,
        .comment      = "Set bit 2 in (HL)",
        .handler      = &opcode_cb_set
    },
    {
        .mnemonic     = "SET 2,A",
        .opcode       = 0xD7,
        .length       = 2,
        .cycles_true  = 8,
        .cycles_false = 8,
        .comment      = "Set bit 2 in A",
        .handler      = &opcode_cb_set
    },
    {
        .mnemonic     = "SET 3,B",
        .opcode       = 0xD8,
        .length       = 2,
        .cycles_true  = 8,
        .cycles_false = 8,
        .comment      = "Set bit 3 in B",
        .handler      = &opcode_cb_set
    },
    {
        .mnemonic     = "SET 3,C",
        .opcode       = 0xD9,
        .length       = 2,
        .cycles_true  = 8,
        .cycles_false = 8,
        .comment      = "Set bit 3 in C",
        .handler      = &opcode_cb_set
    },
    {
        .mnemonic     = "SET 3,D",
        .opcode       = 0xDA,
        .length       = 2,
        .cycles_true  = 8,
        .cycles_false = 8,
        .comment      = "Set bit 3 in D",
        .handler      = &opcode_cb_set
    },
    {
        .mnemonic     = "SET 3,E",
        .opcode       = 0xDB,
        .length       = 2,
        .cycles_true  = 8,
        .cycles_false = 8,
        .comment      = "Set bit 3 in E",
        .handler      = &opcode_cb_set
    },
    {
        .mnemonic     = "SET 3,H",
        .opcode       = 0xDC,
        .length       = 2,
        .cycles_true  = 8,
        .cycles_false = 8,
        .comment      = "Set bit 3 in H",
        .handler      = &opcode_cb_set
    },
    {
        .mnemonic     = "SET 3,L",
        .opcode       = 0xDD,
        .length       = 2,
        .cycles_true  = 8,
        .cycles_false = 8,
        .comment      = "Set bit 3 in L",
        .handler      = &opcode_cb_set
    },
    {
        .mnemonic     = "SET 3,(HL)",
        .opcode       = 0xDE,
        .length       = 2,
        .cycles_true  = 16,
        .cycles_false = 16,
        .comment      = "Set bit 3 in (HL)",
        .handler      = &opcode_cb_set
    },
    {
        .mnemonic     = "SET 3,A",
        .opcode       = 0xDF,
        .length       = 2,
        .cycles_true  = 8,
        .cycles_false = 8,
        .comment      = "Set bit 3 in A",
        .handler      = &opcode_cb_set
    },
    {
        .mnemonic     = "SET 4,B",
        .opcode       = 0xE0,
        .length       = 2,
        .cycles_true  = 8,
        .cycles_false = 8,
        .comment      = "Set bit 4 in B",
        .handler      = &opcode_cb_set
    },
    {
        .mnemonic     = "SET 4,C",
        .opcode       = 0xE1,
        .length       = 2,
        .cycles_true  = 8,
        .cycles_false = 8,
        .comment      = "Set bit 4 in C",
        .handler      = &opcode_cb_set
    },
    {
        .mnemonic     = "SET 4,D",
        .opcode       = 0xE2,
        .length       = 2,
        .cycles_true  = 8,
        .cycles_false = 8,
        .comment      = "Set bit 4 in D",
        .handler      = &opcode_cb_set
    },
    {
        .mnemonic     = "SET 4,E",
        .opcode       = 0xE3,
        .length       = 2,
        .cycles_true  = 8,
        .cycles_false = 8,
        .comment      = "Set bit 4 in E",
        .handler      = &opcode_cb_set
    },
    {
        .mnemonic     = "SET 4,H",
        .opcode       = 0xE4,
        .length       = 2,
        .cycles_true  = 8,
        .cycles_false = 8,
        .comment      = "Set bit 4 in H",
        .handler      = &opcode_cb_set
    },
    {
        .mnemonic     = "SET 4,L",
        .opcode       = 0xE5,
        .length       = 2,
        .cycles_true  = 8,
        .cycles_false = 8,
        .comment      = "Set bit 4 in L",
        .handler      = &opcode_cb_set
    },
    {
        .mnemonic     = "SET 4,(HL)",
        .opcode       = 0xE6,
        .length       = 2,
        .cycles_true  = 16,
        .cycles_false = 16,
        .comment      = "Set bit 4 in (HL)",
        .handler      = &opcode_cb_set
    },
    {
        .mnemonic     = "SET 4,A",
        .opcode       = 0xE7,
        .length       = 2,
        .cycles_true  = 8,
        .cycles_false = 8,
        .comment      = "Set bit 4 in A",
        .handler      = &opcode_cb_set
    },
    {
        .mnemonic     = "SET 5,B",
        .opcode       = 0xE8,
        .length       = 2,
        .cycles_true  = 8,
        .cycles_false = 8,
        .comment      = "Set bit 5 in B",
        .handler      = &opcode_cb_set
    },
    {
        .mnemonic     = "SET 5,C",
        .opcode       = 0xE9,
        .length       = 2,
        .cycles_true  = 8,
        .cycles_false = 8,
        .comment      = "Set bit 5 in C",
        .handler      = &opcode_cb_set
    },
    {
        .mnemonic     = "SET 5,D",
        .opcode       = 0xEA,
        .length       = 2,
        .cycles_true  = 8,
        .cycles_false = 8,
        .comment      = "Set bit 5 in D",
        .handler      = &opcode_cb_set
    },
    {
        .mnemonic     = "SET 5,E",
        .opcode       = 0xEB,
        .length       = 2,
        .cycles_true  = 8,
        .cycles_false = 8,
        .comment      = "Set bit 5 in E",
        .handler      = &opcode_cb_set
    },
    {
        .mnemonic     = "SET 5,H",
        .opcode       = 0xEC,
        .length       = 2,
        .cycles_true  = 8,
        .cycles_false = 8,
        .comment      = "Set bit 5 in H",
        .handler      = &opcode_cb_set
    },
    {
        .mnemonic     = "SET 5,L",
        .opcode       = 0xED,
        .length       = 2,
        .cycles_true  = 8,
        .cycles_false = 8,
        .comment      = "Set bit 5 in L",
        .handler      = &opcode_cb_set
    },
    {
        .mnemonic     = "SET 5,(HL)",
        .opcode       = 0xEE,
        .length       = 2,
        .cycles_true  = 16,
        .cycles_false = 16,
        .comment      = "Set bit 5 in (HL)",
        .handler      = &opcode_cb_set
    },
    {
        .mnemonic     = "SET 5,A",
        .opcode       = 0xEF,
        .length       = 2,
        .cycles_true  = 8,
        .cycles_false = 8,
        .comment      = "Set bit 5 in A",
        .handler      = &opcode_cb_set
    },
    {
        .mnemonic     = "SET 6,B",
        .opcode       = 0xF0,
        .length       = 2,
        .cycles_true  = 8,
        .cycles_false = 8,
        .comment      = "Set bit 6 in B",
        .handler      = &opcode_cb_set
    },
    {
        .mnemonic     = "SET 6,C",
        .opcode       = 0xF1,
        .length       = 2,
        .cycles_true  = 8,
        .cycles_false = 8,
        .comment      = "Set bit 6 in C",
        .handler      = &opcode_cb_set
    },
    {
        .mnemonic     = "SET 6,D",
        .opcode       = 0xF2,
        .length       = 2,
        .cycles_true  = 8,
        .cycles_false = 8,
        .comment      = "Set bit 6 in D",
        .handler      = &opcode_cb_set
    },
    {
        .mnemonic     = "SET 6,E",
        .opcode       = 0xF3,
        .length       = 2,
        .cycles_true  = 8,
        .cycles_false = 8,
        .comment      = "Set bit 6 in E",
        .handler      = &opcode_cb_set
    },
    {
        .mnemonic     = "SET 6,H",
        .opcode       = 0xF4,
        .length       = 2,
        .cycles_true  = 8,
        .cycles_false = 8,
        .comment      = "Set bit 6 in H",
        .handler      = &opcode_cb_set
    },
    {
        .mnemonic     = "SET 6,L",
        .opcode       = 0xF5,
        .length       = 2,
        .cycles_true  = 8,
        .cycles_false = 8,
        .comment      = "Set bit 6 in L",
        .handler      = &opcode_cb_set
    },
    {
        .mnemonic     = "SET 6,(HL)",
        .opcode       = 0xF6,
        .length       = 2,
        .cycles_true  = 16,
        .cycles_false = 16,
        .comment      = "Set bit 6 in (HL)",
        .handler      = &opcode_cb_set
    },
    {
        .mnemonic     = "SET 6,A",
        .opcode       = 0xF7,
        .length       = 2,
        .cycles_true  = 8,
        .cycles_false = 8,
        .comment      = "Set bit 6 in A",
        .handler      = &opcode_cb_set
    },
    {
        .mnemonic     = "SET 7,B",
        .opcode       = 0xF8,
        .length       = 2,
        .cycles_true  = 8,
        .cycles_false = 8,
        .comment      = "Set bit 7 in B",
        .handler      = &opcode_cb_set
    },
    {
        .mnemonic     = "SET 7,C",
        .opcode       = 0xF9,
        .length       = 2,
        .cycles_true  = 8,
        .cycles_false = 8,
        .comment      = "Set bit 7 in C",
        .handler      = &opcode_cb_set
    },
    {
        .mnemonic     = "SET 7,D",
        .opcode       = 0xFA,
        .length       = 2,
        .cycles_true  = 8,
        .cycles_false = 8,
        .comment      = "Set bit 7 in D",
        .handler      = &opcode_cb_set
    },
    {
        .mnemonic     = "SET 7,E",
        .opcode       = 0xFB,
        .length       = 2,
        .cycles_true  = 8,
        .cycles_false = 8,
        .comment      = "Set bit 7 in E",
        .handler      = &opcode_cb_set
    },
    {
        .mnemonic     = "SET 7,H",
        .opcode       = 0xFC,
        .length       = 2,
        .cycles_true  = 8,
        .cycles_false = 8,
        .comment      = "Set bit 7 in H",
        .handler      = &opcode_cb_set
    },
    {
        .mnemonic     = "SET 7,L",
        .opcode       = 0xFD,
        .length       = 2,
        .cycles_true  = 8,
        .cycles_false = 8,
        .comment      = "Set bit 7 in L",
        .handler      = &opcode_cb_set
    },
    {
        .mnemonic     = "SET 7,(HL)",
        .opcode       = 0xFE,
        .length       = 2,
        .cycles_true  = 16,
        .cycles_false = 16,
        .comment      = "Set bit 7 in (HL)",
        .handler      = &opcode_cb_set
    },
    {
        .mnemonic     = "SET 7,A",
        .opcode       = 0xFF,
        .length       = 2,
        .cycles_true  = 8,
        .cycles_false = 8,
        .comment      = "Set bit 7 in A",
        .handler      = &opcode_cb_set
    }
};