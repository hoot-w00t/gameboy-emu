/*
ld.c
CPU Stack push/pop functions
LD, PUSH, POP opcodes

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
#include "cpu/cpu.h"
#include "cpu/registers.h"
#include "cpu/opcodes/alu/add.h"
#include "mmu/mmu.h"

// Push uint16 to stack
void cpu_push_u16(uint16_t value, gb_system_t *gb)
{
    mmu_write_u16(gb->sp - 2, value, gb);
    gb->sp -= 2;
}

// Pop uint16 from stack
uint16_t cpu_pop_u16(gb_system_t *gb)
{
    uint16_t value = mmu_read_u16(gb->sp, gb);

    gb->sp += 2;
    return value;
}

// PUSH BC
int opcode_push_bc(const opcode_t *opcode, gb_system_t *gb)
{
    cpu_push_u16(reg_read_u16(REG_BC, gb), gb);
    return opcode->cycles_true;
}

// PUSH DE
int opcode_push_de(const opcode_t *opcode, gb_system_t *gb)
{
    cpu_push_u16(reg_read_u16(REG_DE, gb), gb);
    return opcode->cycles_true;
}

// PUSH HL
int opcode_push_hl(const opcode_t *opcode, gb_system_t *gb)
{
    cpu_push_u16(reg_read_u16(REG_HL, gb), gb);
    return opcode->cycles_true;
}

// PUSH AF
int opcode_push_af(const opcode_t *opcode, gb_system_t *gb)
{
    cpu_push_u16(reg_read_u16(REG_AF, gb), gb);
    return opcode->cycles_true;
}

// POP BC
int opcode_pop_bc(const opcode_t *opcode, gb_system_t *gb)
{
    reg_write_u16(REG_BC, cpu_pop_u16(gb), gb);
    return opcode->cycles_true;
}

// POP DE
int opcode_pop_de(const opcode_t *opcode, gb_system_t *gb)
{
    reg_write_u16(REG_DE, cpu_pop_u16(gb), gb);
    return opcode->cycles_true;
}

// POP HL
int opcode_pop_hl(const opcode_t *opcode, gb_system_t *gb)
{
    reg_write_u16(REG_HL, cpu_pop_u16(gb), gb);
    return opcode->cycles_true;
}

// POP AF
int opcode_pop_af(const opcode_t *opcode, gb_system_t *gb)
{
    reg_write_u16(REG_AF, (cpu_pop_u16(gb) & 0xFFF0), gb);
    return opcode->cycles_true;
}

// LD B,n
int opcode_ld_b_n(const opcode_t *opcode, gb_system_t *gb)
{
    reg_writeb(REG_B, cpu_fetchb(gb), gb);
    return opcode->cycles_true;
}

// LD C,n
int opcode_ld_c_n(const opcode_t *opcode, gb_system_t *gb)
{
    reg_writeb(REG_C, cpu_fetchb(gb), gb);
    return opcode->cycles_true;
}

// LD D,n
int opcode_ld_d_n(const opcode_t *opcode, gb_system_t *gb)
{
    reg_writeb(REG_D, cpu_fetchb(gb), gb);
    return opcode->cycles_true;
}

// LD E,n
int opcode_ld_e_n(const opcode_t *opcode, gb_system_t *gb)
{
    reg_writeb(REG_E, cpu_fetchb(gb), gb);
    return opcode->cycles_true;
}

// LD H,n
int opcode_ld_h_n(const opcode_t *opcode, gb_system_t *gb)
{
    reg_writeb(REG_H, cpu_fetchb(gb), gb);
    return opcode->cycles_true;
}

// LD L,n
int opcode_ld_l_n(const opcode_t *opcode, gb_system_t *gb)
{
    reg_writeb(REG_L, cpu_fetchb(gb), gb);
    return opcode->cycles_true;
}

// LD (HL),n
int opcode_ld_hl_n(const opcode_t *opcode, gb_system_t *gb)
{
    mmu_writeb(reg_read_u16(REG_HL, gb), cpu_fetchb(gb), gb);
    return opcode->cycles_true;
}

// LD A,n
int opcode_ld_a_n(const opcode_t *opcode, gb_system_t *gb)
{
    reg_writeb(REG_A, cpu_fetchb(gb), gb);
    return opcode->cycles_true;
}

// LD B,B
int opcode_ld_b_b(const opcode_t *opcode, gb_system_t *gb)
{
    reg_writeb(REG_B, reg_readb(REG_B, gb), gb);
    return opcode->cycles_true;
}

// LD B,C
int opcode_ld_b_c(const opcode_t *opcode, gb_system_t *gb)
{
    reg_writeb(REG_B, reg_readb(REG_C, gb), gb);
    return opcode->cycles_true;
}

// LD B,D
int opcode_ld_b_d(const opcode_t *opcode, gb_system_t *gb)
{
    reg_writeb(REG_B, reg_readb(REG_D, gb), gb);
    return opcode->cycles_true;
}

// LD B,E
int opcode_ld_b_e(const opcode_t *opcode, gb_system_t *gb)
{
    reg_writeb(REG_B, reg_readb(REG_E, gb), gb);
    return opcode->cycles_true;
}

// LD B,H
int opcode_ld_b_h(const opcode_t *opcode, gb_system_t *gb)
{
    reg_writeb(REG_B, reg_readb(REG_H, gb), gb);
    return opcode->cycles_true;
}

// LD B,L
int opcode_ld_b_l(const opcode_t *opcode, gb_system_t *gb)
{
    reg_writeb(REG_B, reg_readb(REG_L, gb), gb);
    return opcode->cycles_true;
}

// LD B,(HL)
int opcode_ld_b_hl(const opcode_t *opcode, gb_system_t *gb)
{
    reg_writeb(REG_B, mmu_readb(reg_read_u16(REG_HL, gb), gb), gb);
    return opcode->cycles_true;
}

// LD B,A
int opcode_ld_b_a(const opcode_t *opcode, gb_system_t *gb)
{
    reg_writeb(REG_B, reg_readb(REG_A, gb), gb);
    return opcode->cycles_true;
}

// LD C,B
int opcode_ld_c_b(const opcode_t *opcode, gb_system_t *gb)
{
    reg_writeb(REG_C, reg_readb(REG_B, gb), gb);
    return opcode->cycles_true;
}

// LD C,C
int opcode_ld_c_c(const opcode_t *opcode, gb_system_t *gb)
{
    reg_writeb(REG_C, reg_readb(REG_C, gb), gb);
    return opcode->cycles_true;
}

// LD C,D
int opcode_ld_c_d(const opcode_t *opcode, gb_system_t *gb)
{
    reg_writeb(REG_C, reg_readb(REG_D, gb), gb);
    return opcode->cycles_true;
}

// LD C,E
int opcode_ld_c_e(const opcode_t *opcode, gb_system_t *gb)
{
    reg_writeb(REG_C, reg_readb(REG_E, gb), gb);
    return opcode->cycles_true;
}

// LD C,H
int opcode_ld_c_h(const opcode_t *opcode, gb_system_t *gb)
{
    reg_writeb(REG_C, reg_readb(REG_H, gb), gb);
    return opcode->cycles_true;
}

// LD C,L
int opcode_ld_c_l(const opcode_t *opcode, gb_system_t *gb)
{
    reg_writeb(REG_C, reg_readb(REG_L, gb), gb);
    return opcode->cycles_true;
}

// LD C,(HL)
int opcode_ld_c_hl(const opcode_t *opcode, gb_system_t *gb)
{
    reg_writeb(REG_C, mmu_readb(reg_read_u16(REG_HL, gb), gb), gb);
    return opcode->cycles_true;
}

// LD C,A
int opcode_ld_c_a(const opcode_t *opcode, gb_system_t *gb)
{
    reg_writeb(REG_C, reg_readb(REG_A, gb), gb);
    return opcode->cycles_true;
}

// LD D,B
int opcode_ld_d_b(const opcode_t *opcode, gb_system_t *gb)
{
    reg_writeb(REG_D, reg_readb(REG_B, gb), gb);
    return opcode->cycles_true;
}

// LD D,C
int opcode_ld_d_c(const opcode_t *opcode, gb_system_t *gb)
{
    reg_writeb(REG_D, reg_readb(REG_C, gb), gb);
    return opcode->cycles_true;
}

// LD D,D
int opcode_ld_d_d(const opcode_t *opcode, gb_system_t *gb)
{
    reg_writeb(REG_D, reg_readb(REG_D, gb), gb);
    return opcode->cycles_true;
}

// LD D,E
int opcode_ld_d_e(const opcode_t *opcode, gb_system_t *gb)
{
    reg_writeb(REG_D, reg_readb(REG_E, gb), gb);
    return opcode->cycles_true;
}

// LD D,H
int opcode_ld_d_h(const opcode_t *opcode, gb_system_t *gb)
{
    reg_writeb(REG_D, reg_readb(REG_H, gb), gb);
    return opcode->cycles_true;
}

// LD D,L
int opcode_ld_d_l(const opcode_t *opcode, gb_system_t *gb)
{
    reg_writeb(REG_D, reg_readb(REG_L, gb), gb);
    return opcode->cycles_true;
}

// LD D,(HL)
int opcode_ld_d_hl(const opcode_t *opcode, gb_system_t *gb)
{
    reg_writeb(REG_D, mmu_readb(reg_read_u16(REG_HL, gb), gb), gb);
    return opcode->cycles_true;
}

// LD D,A
int opcode_ld_d_a(const opcode_t *opcode, gb_system_t *gb)
{
    reg_writeb(REG_D, reg_readb(REG_A, gb), gb);
    return opcode->cycles_true;
}

// LD E,B
int opcode_ld_e_b(const opcode_t *opcode, gb_system_t *gb)
{
    reg_writeb(REG_E, reg_readb(REG_B, gb), gb);
    return opcode->cycles_true;
}

// LD E,C
int opcode_ld_e_c(const opcode_t *opcode, gb_system_t *gb)
{
    reg_writeb(REG_E, reg_readb(REG_C, gb), gb);
    return opcode->cycles_true;
}

// LD E,D
int opcode_ld_e_d(const opcode_t *opcode, gb_system_t *gb)
{
    reg_writeb(REG_E, reg_readb(REG_D, gb), gb);
    return opcode->cycles_true;
}

// LD E,E
int opcode_ld_e_e(const opcode_t *opcode, gb_system_t *gb)
{
    reg_writeb(REG_E, reg_readb(REG_E, gb), gb);
    return opcode->cycles_true;
}

// LD E,H
int opcode_ld_e_h(const opcode_t *opcode, gb_system_t *gb)
{
    reg_writeb(REG_E, reg_readb(REG_H, gb), gb);
    return opcode->cycles_true;
}

// LD E,L
int opcode_ld_e_l(const opcode_t *opcode, gb_system_t *gb)
{
    reg_writeb(REG_E, reg_readb(REG_L, gb), gb);
    return opcode->cycles_true;
}

// LD E,(HL)
int opcode_ld_e_hl(const opcode_t *opcode, gb_system_t *gb)
{
    reg_writeb(REG_E, mmu_readb(reg_read_u16(REG_HL, gb), gb), gb);
    return opcode->cycles_true;
}

// LD E,A
int opcode_ld_e_a(const opcode_t *opcode, gb_system_t *gb)
{
    reg_writeb(REG_E, reg_readb(REG_A, gb), gb);
    return opcode->cycles_true;
}

// LD H,B
int opcode_ld_h_b(const opcode_t *opcode, gb_system_t *gb)
{
    reg_writeb(REG_H, reg_readb(REG_B, gb), gb);
    return opcode->cycles_true;
}

// LD H,C
int opcode_ld_h_c(const opcode_t *opcode, gb_system_t *gb)
{
    reg_writeb(REG_H, reg_readb(REG_C, gb), gb);
    return opcode->cycles_true;
}

// LD H,D
int opcode_ld_h_d(const opcode_t *opcode, gb_system_t *gb)
{
    reg_writeb(REG_H, reg_readb(REG_D, gb), gb);
    return opcode->cycles_true;
}

// LD H,E
int opcode_ld_h_e(const opcode_t *opcode, gb_system_t *gb)
{
    reg_writeb(REG_H, reg_readb(REG_E, gb), gb);
    return opcode->cycles_true;
}

// LD H,H
int opcode_ld_h_h(const opcode_t *opcode, gb_system_t *gb)
{
    reg_writeb(REG_H, reg_readb(REG_H, gb), gb);
    return opcode->cycles_true;
}

// LD H,L
int opcode_ld_h_l(const opcode_t *opcode, gb_system_t *gb)
{
    reg_writeb(REG_H, reg_readb(REG_L, gb), gb);
    return opcode->cycles_true;
}

// LD H,(HL)
int opcode_ld_h_hl(const opcode_t *opcode, gb_system_t *gb)
{
    reg_writeb(REG_H, mmu_readb(reg_read_u16(REG_HL, gb), gb), gb);
    return opcode->cycles_true;
}

// LD H,A
int opcode_ld_h_a(const opcode_t *opcode, gb_system_t *gb)
{
    reg_writeb(REG_H, reg_readb(REG_A, gb), gb);
    return opcode->cycles_true;
}

// LD L,B
int opcode_ld_l_b(const opcode_t *opcode, gb_system_t *gb)
{
    reg_writeb(REG_L, reg_readb(REG_B, gb), gb);
    return opcode->cycles_true;
}

// LD L,C
int opcode_ld_l_c(const opcode_t *opcode, gb_system_t *gb)
{
    reg_writeb(REG_L, reg_readb(REG_C, gb), gb);
    return opcode->cycles_true;
}

// LD L,D
int opcode_ld_l_d(const opcode_t *opcode, gb_system_t *gb)
{
    reg_writeb(REG_L, reg_readb(REG_D, gb), gb);
    return opcode->cycles_true;
}

// LD L,E
int opcode_ld_l_e(const opcode_t *opcode, gb_system_t *gb)
{
    reg_writeb(REG_L, reg_readb(REG_E, gb), gb);
    return opcode->cycles_true;
}

// LD L,H
int opcode_ld_l_h(const opcode_t *opcode, gb_system_t *gb)
{
    reg_writeb(REG_L, reg_readb(REG_H, gb), gb);
    return opcode->cycles_true;
}

// LD L,L
int opcode_ld_l_l(const opcode_t *opcode, gb_system_t *gb)
{
    reg_writeb(REG_L, reg_readb(REG_L, gb), gb);
    return opcode->cycles_true;
}

// LD L,(HL)
int opcode_ld_l_hl(const opcode_t *opcode, gb_system_t *gb)
{
    reg_writeb(REG_L, mmu_readb(reg_read_u16(REG_HL, gb), gb), gb);
    return opcode->cycles_true;
}

// LD L,A
int opcode_ld_l_a(const opcode_t *opcode, gb_system_t *gb)
{
    reg_writeb(REG_L, reg_readb(REG_A, gb), gb);
    return opcode->cycles_true;
}

// LD (HL),B
int opcode_ld_hl_b(const opcode_t *opcode, gb_system_t *gb)
{
    mmu_writeb(reg_read_u16(REG_HL, gb), reg_readb(REG_B, gb), gb);
    return opcode->cycles_true;
}

// LD (HL),C
int opcode_ld_hl_c(const opcode_t *opcode, gb_system_t *gb)
{
    mmu_writeb(reg_read_u16(REG_HL, gb), reg_readb(REG_C, gb), gb);
    return opcode->cycles_true;
}

// LD (HL),D
int opcode_ld_hl_d(const opcode_t *opcode, gb_system_t *gb)
{
    mmu_writeb(reg_read_u16(REG_HL, gb), reg_readb(REG_D, gb), gb);
    return opcode->cycles_true;
}

// LD (HL),E
int opcode_ld_hl_e(const opcode_t *opcode, gb_system_t *gb)
{
    mmu_writeb(reg_read_u16(REG_HL, gb), reg_readb(REG_E, gb), gb);
    return opcode->cycles_true;
}

// LD (HL),H
int opcode_ld_hl_h(const opcode_t *opcode, gb_system_t *gb)
{
    mmu_writeb(reg_read_u16(REG_HL, gb), reg_readb(REG_H, gb), gb);
    return opcode->cycles_true;
}

// LD (HL),L
int opcode_ld_hl_l(const opcode_t *opcode, gb_system_t *gb)
{
    mmu_writeb(reg_read_u16(REG_HL, gb), reg_readb(REG_L, gb), gb);
    return opcode->cycles_true;
}

// LD (HL),A
int opcode_ld_hl_a(const opcode_t *opcode, gb_system_t *gb)
{
    mmu_writeb(reg_read_u16(REG_HL, gb), reg_readb(REG_A, gb), gb);
    return opcode->cycles_true;
}

// LD A,B
int opcode_ld_a_b(const opcode_t *opcode, gb_system_t *gb)
{
    reg_writeb(REG_A, reg_readb(REG_B, gb), gb);
    return opcode->cycles_true;
}

// LD A,C
int opcode_ld_a_c(const opcode_t *opcode, gb_system_t *gb)
{
    reg_writeb(REG_A, reg_readb(REG_C, gb), gb);
    return opcode->cycles_true;
}

// LD A,D
int opcode_ld_a_d(const opcode_t *opcode, gb_system_t *gb)
{
    reg_writeb(REG_A, reg_readb(REG_D, gb), gb);
    return opcode->cycles_true;
}

// LD A,E
int opcode_ld_a_e(const opcode_t *opcode, gb_system_t *gb)
{
    reg_writeb(REG_A, reg_readb(REG_E, gb), gb);
    return opcode->cycles_true;
}

// LD A,H
int opcode_ld_a_h(const opcode_t *opcode, gb_system_t *gb)
{
    reg_writeb(REG_A, reg_readb(REG_H, gb), gb);
    return opcode->cycles_true;
}

// LD A,L
int opcode_ld_a_l(const opcode_t *opcode, gb_system_t *gb)
{
    reg_writeb(REG_A, reg_readb(REG_L, gb), gb);
    return opcode->cycles_true;
}

// LD A,(HL)
int opcode_ld_a_hl(const opcode_t *opcode, gb_system_t *gb)
{
    reg_writeb(REG_A, mmu_readb(reg_read_u16(REG_HL, gb), gb), gb);
    return opcode->cycles_true;
}

// LD A,A
int opcode_ld_a_a(const opcode_t *opcode, gb_system_t *gb)
{
    reg_writeb(REG_A, reg_readb(REG_A, gb), gb);
    return opcode->cycles_true;
}

// LD A,(BC)
int opcode_ld_a_bc(const opcode_t *opcode, gb_system_t *gb)
{
    reg_writeb(REG_A, mmu_readb(reg_read_u16(REG_BC, gb), gb), gb);
    return opcode->cycles_true;
}

// LD A,(DE)
int opcode_ld_a_de(const opcode_t *opcode, gb_system_t *gb)
{
    reg_writeb(REG_A, mmu_readb(reg_read_u16(REG_DE, gb), gb), gb);
    return opcode->cycles_true;
}

// LD A,(nn)
int opcode_ld_a_nn(const opcode_t *opcode, gb_system_t *gb)
{
    reg_writeb(REG_A, mmu_readb(cpu_fetch_u16(gb), gb), gb);
    return opcode->cycles_true;
}

// LD (BC),A
int opcode_ld_bc_a(const opcode_t *opcode, gb_system_t *gb)
{
    mmu_writeb(reg_read_u16(REG_BC, gb), reg_readb(REG_A, gb), gb);
    return opcode->cycles_true;
}

// LD (DE),A
int opcode_ld_de_a(const opcode_t *opcode, gb_system_t *gb)
{
    mmu_writeb(reg_read_u16(REG_DE, gb), reg_readb(REG_A, gb), gb);
    return opcode->cycles_true;
}

// LD (nn),A
int opcode_ld_nn_a(const opcode_t *opcode, gb_system_t *gb)
{
    mmu_writeb(cpu_fetch_u16(gb), reg_readb(REG_A, gb), gb);
    return opcode->cycles_true;
}

// LD A,($FF00+C) or LD A,(C)
int opcode_ld_a_ff00_c(const opcode_t *opcode, gb_system_t *gb)
{
    reg_writeb(REG_A, mmu_readb(0xFF00 + reg_readb(REG_C, gb), gb), gb);
    return opcode->cycles_true;
}

// LD ($FF00+C),A or LD (C),A
int opcode_ld_ff00_c_a(const opcode_t *opcode, gb_system_t *gb)
{
    mmu_writeb(0xFF00 + reg_readb(REG_C, gb), reg_readb(REG_A, gb), gb);
    return opcode->cycles_true;
}

// LDD A,(HL)
int opcode_ldd_a_hl(const opcode_t *opcode, gb_system_t *gb)
{
    reg_writeb(REG_A, mmu_readb(reg_read_u16(REG_HL, gb), gb), gb);
    reg_write_u16(REG_HL, reg_read_u16(REG_HL, gb) - 1, gb);
    return opcode->cycles_true;
}

// LDD (HL),A
int opcode_ldd_hl_a(const opcode_t *opcode, gb_system_t *gb)
{
    mmu_writeb(reg_read_u16(REG_HL, gb), reg_readb(REG_A, gb), gb);
    reg_write_u16(REG_HL, reg_read_u16(REG_HL, gb) - 1, gb);
    return opcode->cycles_true;
}

// LDI A,(HL)
int opcode_ldi_a_hl(const opcode_t *opcode, gb_system_t *gb)
{
    reg_writeb(REG_A, mmu_readb(reg_read_u16(REG_HL, gb), gb), gb);
    reg_write_u16(REG_HL, reg_read_u16(REG_HL, gb) + 1, gb);
    return opcode->cycles_true;
}

// LDI (HL),A
int opcode_ldi_hl_a(const opcode_t *opcode, gb_system_t *gb)
{
    mmu_writeb(reg_read_u16(REG_HL, gb), reg_readb(REG_A, gb), gb);
    reg_write_u16(REG_HL, reg_read_u16(REG_HL, gb) + 1, gb);
    return opcode->cycles_true;
}

// LDH (n),A
int opcode_ldh_n_a(const opcode_t *opcode, gb_system_t *gb)
{
    mmu_writeb(0xFF00 + cpu_fetchb(gb), reg_readb(REG_A, gb), gb);
    return opcode->cycles_true;
}

// LDH A,(n)
int opcode_ldh_a_n(const opcode_t *opcode, gb_system_t *gb)
{
    reg_writeb(REG_A, mmu_readb(0xFF00 + cpu_fetchb(gb), gb), gb);
    return opcode->cycles_true;
}

// LD BC,nn
int opcode_ld_bc_nn(const opcode_t *opcode, gb_system_t *gb)
{
    reg_write_u16(REG_BC, cpu_fetch_u16(gb), gb);
    return opcode->cycles_true;
}

// LD DE,nn
int opcode_ld_de_nn(const opcode_t *opcode, gb_system_t *gb)
{
    reg_write_u16(REG_DE, cpu_fetch_u16(gb), gb);
    return opcode->cycles_true;
}

// LD HL,nn
int opcode_ld_hl_nn(const opcode_t *opcode, gb_system_t *gb)
{
    reg_write_u16(REG_HL, cpu_fetch_u16(gb), gb);
    return opcode->cycles_true;
}

// LD SP,nn
int opcode_ld_sp_nn(const opcode_t *opcode, gb_system_t *gb)
{
    gb->sp = cpu_fetch_u16(gb);
    return opcode->cycles_true;
}

// LD SP,HL
int opcode_ld_sp_hl(const opcode_t *opcode, gb_system_t *gb)
{
    gb->sp = reg_read_u16(REG_HL, gb);
    return opcode->cycles_true;
}

// LDHL SP,e
int opcode_ld_sp_e(const opcode_t *opcode, gb_system_t *gb)
{
    reg_write_u16(REG_HL, cpu_add_sp_e((sbyte_t) cpu_fetchb(gb), gb), gb);
    return opcode->cycles_true;
}

// LD (nn),SP
int opcode_ld_nn_sp(const opcode_t *opcode, gb_system_t *gb)
{
    mmu_write_u16(cpu_fetch_u16(gb), gb->sp, gb);
    return opcode->cycles_true;
}