/*
control.c
Control opcodes

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
#include "cpu/registers.h"

int opcode_nop(const opcode_t *opcode, __attribute__((unused)) gb_system_t *gb)
{
    return opcode->cycles_true;
}

// TODO: Enable/Disable interrupts after the instruction cycles are elasped

int opcode_ei(const opcode_t *opcode, gb_system_t *gb)
{
    gb->interrupts.ime = IME_ENABLE;
    return opcode->cycles_true;
}

int opcode_di(const opcode_t *opcode, gb_system_t *gb)
{
    gb->interrupts.ime = IME_DISABLE;
    return opcode->cycles_true;
}

int opcode_daa(const opcode_t *opcode, gb_system_t *gb)
{
    byte_t A = reg_readb(REG_A, gb);
    byte_t lo = A & 0xF;
    byte_t hi = (A >> 4) & 0xF;
    bool Cy = reg_flag(FLAG_CY, gb);
    bool H = reg_flag(FLAG_H, gb);
    bool Cy_new = Cy;

    // TODO: Optimize this
    if (reg_flag(FLAG_N, gb)) {
        // Previous instruction was SUB/SBC
        if (!Cy && hi <= 0x9 && !H && lo <= 0x9) {
            Cy_new = false;
        } else if (!Cy && hi <= 0x8 && H && lo >= 0x6) {
            Cy_new = false;
            A += 0xFA;
        } else if (Cy && hi >= 0x7 && !H && lo <= 0x9) {
            Cy_new = true;
            A += 0xA0;
        } else if (Cy && hi >= 0x6 && H && lo >= 0x6) {
            Cy_new = true;
            A += 0x9A;
        }
    } else {
        // Previous instruction was ADD/ADC
        if (!Cy && hi <= 0x9 && !H && lo <= 0x9) {
            Cy_new = false;
        } else if (!Cy && hi <= 0x8 && !H && lo >= 0xA) {
            Cy_new = false;
            A += 0x06;
        } else if (!Cy && hi <= 0x9 && H && lo <= 0x3) {
            Cy_new = false;
            A += 0x06;
        } else if (!Cy && hi >= 0xA && !H && lo <= 0x9) {
            Cy_new = true;
            A += 0x60;
        } else if (!Cy && hi >= 0x9 && !H && lo >= 0xA) {
            Cy_new = true;
            A += 0x66;
        } else if (!Cy && hi >= 0xA && H && lo <= 0x3) {
            Cy_new = true;
            A += 0x66;
        } else if (Cy && hi <= 0x2 && !H && lo <= 0x9) {
            Cy_new = true;
            A += 0x60;
        } else if (Cy && hi <= 0x2 && !H && lo >= 0xA) {
            Cy_new = true;
            A += 0x66;
        } else if (Cy && hi <= 0x3 && H && lo <= 0x3) {
            Cy_new = true;
            A += 0x66;
        }
    }

    if (A == 0) reg_flag_set(FLAG_Z, gb); else reg_flag_clear(FLAG_Z, gb);
    reg_flag_clear(FLAG_H, gb);
    if (Cy_new) reg_flag_set(FLAG_CY, gb); else reg_flag_clear(FLAG_CY, gb);
    reg_writeb(REG_A, A, gb);

    return opcode->cycles_true;
}