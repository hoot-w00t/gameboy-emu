/*
calls.c
CALL, RST and RET(I) opcodes

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
#include "cpu/opcodes/ld.h"

// Call addr
void cpu_call(uint16_t addr, gb_system_t *gb)
{
    cpu_push_u16(gb->pc, gb);
    gb->pc = addr;
}

// Return from call
void cpu_ret(gb_system_t *gb)
{
    gb->pc = cpu_pop_u16(gb);
}

// CALL nn and CALL cc,nn opcodes
int opcode_call(const opcode_t *opcode, gb_system_t *gb)
{
    uint16_t addr = cpu_fetch_u16(gb);

    switch (opcode->opcode) {
        // CALL nn
        case 0xCD: cpu_call(addr, gb); return opcode->cycles_true;

        // CALL NZ,nn
        case 0xC4:
            if (reg_flag(FLAG_Z, gb) == false) {
                cpu_call(addr, gb);
                return opcode->cycles_true;
            } else {
                return opcode->cycles_false;
            }

        // CALL Z,nn
        case 0xCC:
            if (reg_flag(FLAG_Z, gb) == true) {
                cpu_call(addr, gb);
                return opcode->cycles_true;
            } else {
                return opcode->cycles_false;
            }

        // CALL NC,nn
        case 0xD4:
            if (reg_flag(FLAG_C, gb) == false) {
                cpu_call(addr, gb);
                return opcode->cycles_true;
            } else {
                return opcode->cycles_false;
            }

        // CALL C,nn
        case 0xDC:
            if (reg_flag(FLAG_C, gb) == true) {
                cpu_call(addr, gb);
                return opcode->cycles_true;
            } else {
                return opcode->cycles_false;
            }

        default: return OPCODE_ILLEGAL;
    }
}