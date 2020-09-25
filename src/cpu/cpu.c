/*
cpu.c
GameBoy CPU

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
#include "cpu/opcodes.h"
#include "mmu/mmu.h"

// Emulate a GameBoy CPU cycle
// If emulate_cycles is false, one cycle == one instruction
// Returns 0 on normal operation
// Returns < 0 when system should be halted
//      -1: illegal opcode
//      -2: halt the system
int cpu_cycle(const bool emulate_cycles, gb_system_t *gb)
{
    byte_t opcode_value;
    const opcode_t *opcode;
    int handler_ret;

    // Emulate real CPU cycles
    if (gb->idle_cycles > 0 && emulate_cycles) {
        gb->idle_cycles -= 1;
        gb->cycle_nb += 1;
        return 0;

    } else if (gb->idle_cycles > 0 && !emulate_cycles) {
        // Reset idle cycles if emulate_cycles is false
        gb->idle_cycles = 0;

    }

    // Fetch and execute opcode
    opcode_value = mmu_readb(gb->pc, gb);
    if ((opcode = opcode_identify(opcode_value))) {
        logger(LOG_DEBUG,
               "$%04X: $%02X: %s",
               gb->pc,
               opcode_value,
               opcode->mnemonic);

        handler_ret = (*opcode->handler)(opcode, gb);
    } else {
        handler_ret = OPCODE_ILLEGAL;
    }

    if (handler_ret == OPCODE_ILLEGAL) {
        logger(LOG_CRIT,
               "$%04X: $%02X: Illegal opcode",
               gb->pc,
               opcode_value);

        return -1;
    }

    if (emulate_cycles) {
        gb->cycle_nb += 1;
        if (handler_ret & OPCODE_ACTION) {
            gb->idle_cycles += opcode->cycles_true - 1;
        } else if (handler_ret & OPCODE_NOACTION) {
            gb->idle_cycles += opcode->cycles_false - 1;
        }
    } else if (handler_ret & OPCODE_ACTION) {
        gb->cycle_nb += opcode->cycles_true;
    } else if (handler_ret & OPCODE_NOACTION) {
        gb->cycle_nb += opcode->cycles_false;
    }

    if (!(handler_ret & OPCODE_NOPC)) gb->pc += opcode->length;
    if (handler_ret & OPCODE_EXIT) return -2;

    return 0;
}