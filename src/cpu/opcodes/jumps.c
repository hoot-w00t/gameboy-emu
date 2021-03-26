/*
jumps.c
JP and JR opcodes

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

// JP nn
int opcode_jp_nn(const opcode_t *opcode, gb_system_t *gb)
{
    gb->pc = cpu_fetch_u16(gb);
    return opcode->cycles_true;
}

// JP NZ,nn
int opcode_jp_nz_nn(const opcode_t *opcode, gb_system_t *gb)
{
    const uint16_t addr = cpu_fetch_u16(gb);

    if (!gb->regs.f.flags.z) {
        gb->pc = addr;
        return opcode->cycles_true;
    }
    return opcode->cycles_false;
}

// JP Z,nn
int opcode_jp_z_nn(const opcode_t *opcode, gb_system_t *gb)
{
    const uint16_t addr = cpu_fetch_u16(gb);

    if (gb->regs.f.flags.z) {
        gb->pc = addr;
        return opcode->cycles_true;
    }
    return opcode->cycles_false;
}

// JP NC,nn
int opcode_jp_nc_nn(const opcode_t *opcode, gb_system_t *gb)
{
    const uint16_t addr = cpu_fetch_u16(gb);

    if (!gb->regs.f.flags.c) {
        gb->pc = addr;
        return opcode->cycles_true;
    }
    return opcode->cycles_false;
}

// JP C,nn
int opcode_jp_c_nn(const opcode_t *opcode, gb_system_t *gb)
{
    const uint16_t addr = cpu_fetch_u16(gb);

    if (gb->regs.f.flags.c) {
        gb->pc = addr;
        return opcode->cycles_true;
    }
    return opcode->cycles_false;
}

// JP (HL) opcode
int opcode_jp_hl(const opcode_t *opcode, gb_system_t *gb)
{
    gb->pc = reg_read_hl(gb);
    return opcode->cycles_true;
}

// JR n
int opcode_jr_n(const opcode_t *opcode, gb_system_t *gb)
{
    gb->pc += (sbyte_t) cpu_fetchb(gb);
    return opcode->cycles_true;
}

// JR NZ,n
int opcode_jr_nz_n(const opcode_t *opcode, gb_system_t *gb)
{
    const sbyte_t n = (sbyte_t) cpu_fetchb(gb);

    if (!gb->regs.f.flags.z) {
        gb->pc += n;
        return opcode->cycles_true;
    }
    return opcode->cycles_false;
}

// JR Z,n
int opcode_jr_z_n(const opcode_t *opcode, gb_system_t *gb)
{
    const sbyte_t n = (sbyte_t) cpu_fetchb(gb);

    if (gb->regs.f.flags.z) {
        gb->pc += n;
        return opcode->cycles_true;
    }
    return opcode->cycles_false;
}

// JR NC,n
int opcode_jr_nc_n(const opcode_t *opcode, gb_system_t *gb)
{
    const sbyte_t n = (sbyte_t) cpu_fetchb(gb);

    if (!gb->regs.f.flags.c) {
        gb->pc += n;
        return opcode->cycles_true;
    }
    return opcode->cycles_false;
}

// JR C,n
int opcode_jr_c_n(const opcode_t *opcode, gb_system_t *gb)
{
    const sbyte_t n = (sbyte_t) cpu_fetchb(gb);

    if (gb->regs.f.flags.c) {
        gb->pc += n;
        return opcode->cycles_true;
    }
    return opcode->cycles_false;
}