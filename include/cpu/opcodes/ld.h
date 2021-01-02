/*
ld.h
Function prototypes for ld.c

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

#ifndef _CPU_OPCODE_LD_H
#define _CPU_OPCODE_LD_H

void cpu_push_u16(uint16_t value, gb_system_t *gb);
uint16_t cpu_pop_u16(gb_system_t *gb);

// PUSH opcodes
int opcode_push_bc(const opcode_t *opcode, gb_system_t *gb);
int opcode_push_de(const opcode_t *opcode, gb_system_t *gb);
int opcode_push_hl(const opcode_t *opcode, gb_system_t *gb);
int opcode_push_af(const opcode_t *opcode, gb_system_t *gb);

// POP opcodes
int opcode_pop_bc(const opcode_t *opcode, gb_system_t *gb);
int opcode_pop_de(const opcode_t *opcode, gb_system_t *gb);
int opcode_pop_hl(const opcode_t *opcode, gb_system_t *gb);
int opcode_pop_af(const opcode_t *opcode, gb_system_t *gb);

// LD r,n opcodes
int opcode_ld_b_n(const opcode_t *opcode, gb_system_t *gb);
int opcode_ld_c_n(const opcode_t *opcode, gb_system_t *gb);
int opcode_ld_d_n(const opcode_t *opcode, gb_system_t *gb);
int opcode_ld_e_n(const opcode_t *opcode, gb_system_t *gb);
int opcode_ld_h_n(const opcode_t *opcode, gb_system_t *gb);
int opcode_ld_l_n(const opcode_t *opcode, gb_system_t *gb);
int opcode_ld_hl_n(const opcode_t *opcode, gb_system_t *gb);
int opcode_ld_a_n(const opcode_t *opcode, gb_system_t *gb);

// LD B,r opcodes
int opcode_ld_b_b(const opcode_t *opcode, gb_system_t *gb);
int opcode_ld_b_c(const opcode_t *opcode, gb_system_t *gb);
int opcode_ld_b_d(const opcode_t *opcode, gb_system_t *gb);
int opcode_ld_b_e(const opcode_t *opcode, gb_system_t *gb);
int opcode_ld_b_h(const opcode_t *opcode, gb_system_t *gb);
int opcode_ld_b_l(const opcode_t *opcode, gb_system_t *gb);
int opcode_ld_b_hl(const opcode_t *opcode, gb_system_t *gb);
int opcode_ld_b_a(const opcode_t *opcode, gb_system_t *gb);

// LD C,r opcodes
int opcode_ld_c_b(const opcode_t *opcode, gb_system_t *gb);
int opcode_ld_c_c(const opcode_t *opcode, gb_system_t *gb);
int opcode_ld_c_d(const opcode_t *opcode, gb_system_t *gb);
int opcode_ld_c_e(const opcode_t *opcode, gb_system_t *gb);
int opcode_ld_c_h(const opcode_t *opcode, gb_system_t *gb);
int opcode_ld_c_l(const opcode_t *opcode, gb_system_t *gb);
int opcode_ld_c_hl(const opcode_t *opcode, gb_system_t *gb);
int opcode_ld_c_a(const opcode_t *opcode, gb_system_t *gb);

// LD D,r opcodes
int opcode_ld_d_b(const opcode_t *opcode, gb_system_t *gb);
int opcode_ld_d_c(const opcode_t *opcode, gb_system_t *gb);
int opcode_ld_d_d(const opcode_t *opcode, gb_system_t *gb);
int opcode_ld_d_e(const opcode_t *opcode, gb_system_t *gb);
int opcode_ld_d_h(const opcode_t *opcode, gb_system_t *gb);
int opcode_ld_d_l(const opcode_t *opcode, gb_system_t *gb);
int opcode_ld_d_hl(const opcode_t *opcode, gb_system_t *gb);
int opcode_ld_d_a(const opcode_t *opcode, gb_system_t *gb);

// LD E,r opcodes
int opcode_ld_e_b(const opcode_t *opcode, gb_system_t *gb);
int opcode_ld_e_c(const opcode_t *opcode, gb_system_t *gb);
int opcode_ld_e_d(const opcode_t *opcode, gb_system_t *gb);
int opcode_ld_e_e(const opcode_t *opcode, gb_system_t *gb);
int opcode_ld_e_h(const opcode_t *opcode, gb_system_t *gb);
int opcode_ld_e_l(const opcode_t *opcode, gb_system_t *gb);
int opcode_ld_e_hl(const opcode_t *opcode, gb_system_t *gb);
int opcode_ld_e_a(const opcode_t *opcode, gb_system_t *gb);

// LD H,r opcodes
int opcode_ld_h_b(const opcode_t *opcode, gb_system_t *gb);
int opcode_ld_h_c(const opcode_t *opcode, gb_system_t *gb);
int opcode_ld_h_d(const opcode_t *opcode, gb_system_t *gb);
int opcode_ld_h_e(const opcode_t *opcode, gb_system_t *gb);
int opcode_ld_h_h(const opcode_t *opcode, gb_system_t *gb);
int opcode_ld_h_l(const opcode_t *opcode, gb_system_t *gb);
int opcode_ld_h_hl(const opcode_t *opcode, gb_system_t *gb);
int opcode_ld_h_a(const opcode_t *opcode, gb_system_t *gb);

// LD L,r opcodes
int opcode_ld_l_b(const opcode_t *opcode, gb_system_t *gb);
int opcode_ld_l_c(const opcode_t *opcode, gb_system_t *gb);
int opcode_ld_l_d(const opcode_t *opcode, gb_system_t *gb);
int opcode_ld_l_e(const opcode_t *opcode, gb_system_t *gb);
int opcode_ld_l_h(const opcode_t *opcode, gb_system_t *gb);
int opcode_ld_l_l(const opcode_t *opcode, gb_system_t *gb);
int opcode_ld_l_hl(const opcode_t *opcode, gb_system_t *gb);
int opcode_ld_l_a(const opcode_t *opcode, gb_system_t *gb);

// LD (HL),r opcodes
int opcode_ld_hl_b(const opcode_t *opcode, gb_system_t *gb);
int opcode_ld_hl_c(const opcode_t *opcode, gb_system_t *gb);
int opcode_ld_hl_d(const opcode_t *opcode, gb_system_t *gb);
int opcode_ld_hl_e(const opcode_t *opcode, gb_system_t *gb);
int opcode_ld_hl_h(const opcode_t *opcode, gb_system_t *gb);
int opcode_ld_hl_l(const opcode_t *opcode, gb_system_t *gb);
int opcode_ld_hl_a(const opcode_t *opcode, gb_system_t *gb);

// LD A,r opcodes
int opcode_ld_a_b(const opcode_t *opcode, gb_system_t *gb);
int opcode_ld_a_c(const opcode_t *opcode, gb_system_t *gb);
int opcode_ld_a_d(const opcode_t *opcode, gb_system_t *gb);
int opcode_ld_a_e(const opcode_t *opcode, gb_system_t *gb);
int opcode_ld_a_h(const opcode_t *opcode, gb_system_t *gb);
int opcode_ld_a_l(const opcode_t *opcode, gb_system_t *gb);
int opcode_ld_a_hl(const opcode_t *opcode, gb_system_t *gb);
int opcode_ld_a_a(const opcode_t *opcode, gb_system_t *gb);

// LD A,(nn) opcodes
int opcode_ld_a_bc(const opcode_t *opcode, gb_system_t *gb);
int opcode_ld_a_de(const opcode_t *opcode, gb_system_t *gb);
int opcode_ld_a_nn(const opcode_t *opcode, gb_system_t *gb);

// LD (nn),A opcodes
int opcode_ld_bc_a(const opcode_t *opcode, gb_system_t *gb);
int opcode_ld_de_a(const opcode_t *opcode, gb_system_t *gb);
int opcode_ld_nn_a(const opcode_t *opcode, gb_system_t *gb);

// LD A,($FF00+C)
int opcode_ld_a_ff00_c(const opcode_t *opcode, gb_system_t *gb);

// LD ($FF00+C),A
int opcode_ld_ff00_c_a(const opcode_t *opcode, gb_system_t *gb);

// LDD A,(HL)
int opcode_ldd_a_hl(const opcode_t *opcode, gb_system_t *gb);

// LDD (HL),A
int opcode_ldd_hl_a(const opcode_t *opcode, gb_system_t *gb);

// LDI A,(HL)
int opcode_ldi_a_hl(const opcode_t *opcode, gb_system_t *gb);

// LDI (HL),A
int opcode_ldi_hl_a(const opcode_t *opcode, gb_system_t *gb);

// LDH (n),A
int opcode_ldh_n_a(const opcode_t *opcode, gb_system_t *gb);

// LDH A,(n)
int opcode_ldh_a_n(const opcode_t *opcode, gb_system_t *gb);

// LD r16,nn opcodes
int opcode_ld_bc_nn(const opcode_t *opcode, gb_system_t *gb);
int opcode_ld_de_nn(const opcode_t *opcode, gb_system_t *gb);
int opcode_ld_hl_nn(const opcode_t *opcode, gb_system_t *gb);

// SP related LD opcodes
int opcode_ld_sp_nn(const opcode_t *opcode, gb_system_t *gb);
int opcode_ld_sp_hl(const opcode_t *opcode, gb_system_t *gb);
int opcode_ld_sp_e(const opcode_t *opcode, gb_system_t *gb);
int opcode_ld_nn_sp(const opcode_t *opcode, gb_system_t *gb);

#endif