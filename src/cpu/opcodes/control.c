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

#include "logger.h"
#include "gameboy.h"
#include "cpu/cpu.h"
#include "cpu/registers.h"
#include "cpu/opcodes.h"

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

int opcode_halt(const opcode_t *opcode, gb_system_t *gb)
{
    gb->halt = true;
    return opcode->cycles_true;
}

int opcode_stop(const opcode_t *opcode, gb_system_t *gb)
{
    // TODO: Properly implement STOP when button input is handled
    // Stop only if all IE flags are reset
    //              inputs P10-P13 LOW
    gb->stop = false;
    return opcode->cycles_true;
}

int opcode_daa(const opcode_t *opcode, gb_system_t *gb)
{
    uint16_t result = reg_readb(REG_A, gb);

    if (reg_flag(FLAG_N, gb)) {
        // Previous instruction was SUB/SBC
        if (reg_flag(FLAG_H, gb)) result = (result - 0x06) & 0xFF;
        if (reg_flag(FLAG_C, gb)) result -= 0x60;
    } else {
        // Previous instruction was ADD/ADC
        if (reg_flag(FLAG_H, gb) || (result & 0xF) > 9) result += 0x06;
        if (reg_flag(FLAG_C, gb) || result > 0x9F) result += 0x60;
    }

    ((byte_t) result) ? reg_flag_clear(FLAG_Z, gb) : reg_flag_set(FLAG_Z, gb);
    reg_flag_clear(FLAG_H, gb);
    if (result > 0xFF) reg_flag_set(FLAG_C, gb);
    reg_writeb(REG_A, (byte_t) result, gb);

    return opcode->cycles_true;
}

int opcode_cpl(const opcode_t *opcode, gb_system_t *gb)
{
    reg_writeb(REG_A, reg_readb(REG_A, gb) ^ 0xFF, gb);
    reg_flag_set(FLAG_N, gb);
    reg_flag_set(FLAG_H, gb);

    return opcode->cycles_true;
}

int opcode_ccf(const opcode_t *opcode, gb_system_t *gb)
{
    reg_flag_clear(FLAG_N, gb);
    reg_flag_clear(FLAG_H, gb);
    if (reg_flag(FLAG_C, gb)) reg_flag_clear(FLAG_C, gb); else reg_flag_set(FLAG_C, gb);

    return opcode->cycles_true;
}

int opcode_scf(const opcode_t *opcode, gb_system_t *gb)
{
    reg_flag_clear(FLAG_N, gb);
    reg_flag_clear(FLAG_H, gb);
    reg_flag_set(FLAG_C, gb);

    return opcode->cycles_true;
}