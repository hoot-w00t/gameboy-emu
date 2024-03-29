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
#include "cpu/registers.h"
#include "cpu/interrupts.h"
#include "mmu/mmu.h"
#include "timer.h"
#include <stdio.h>

// Fetch byte from PC and increment PC
byte_t cpu_fetchb(gb_system_t *gb)
{
    byte_t value = mmu_readb(gb->pc, gb);

    gb->pc += 1;
    return value;
}

// Fetch uint16 from PC and increment PC twice
uint16_t cpu_fetch_u16(gb_system_t *gb)
{
    uint16_t value = mmu_read_u16(gb->pc, gb);

    gb->pc += 2;
    return value;
}

// Emulate a GameBoy CPU cycle
// On normal operation, returns the number of CPU cycles
// an instruction will take (or 0 if idling)
// Returns < 0 if opcode didn't execute as normal
//      OPCODE_ILLEGAL: illegal opcode
//      OPCODE_EXIT   : break the emulation loop
int cpu_cycle(gb_system_t *gb)
{
    bool CB;
    byte_t opcode_value;
    const opcode_t *opcode;
    int handler_ret;

    // Emulate the built-in timers
    timer_cycle(gb);

    // Emulate real CPU cycles
    if (gb->idle_cycles > 0) {
        gb->idle_cycles -= 1;
        return 0;
    }

    // Execute ISR if an enabled interrupt is requested
    handler_ret = cpu_int_isr(gb);
    CB = false;
    opcode_value = 0;

    if (!handler_ret && (gb->halt || gb->stop)) {
        // CPU Halted
        return 0;

    } else if (!handler_ret) {
        // No ISR executed, continue on normal operation
        // Fetch and execute opcode
        if ((opcode_value = cpu_fetchb(gb)) == 0xCB) {
            CB = true;
            opcode_value = cpu_fetchb(gb);
            opcode = opcode_cb_identify(opcode_value);
        } else {
            opcode = opcode_identify(opcode_value);
        }

        if (opcode) {
            if (CB) {
                logger(LOG_DEBUG,
                    "$%04X: CB $%02X: %s",
                    gb->pc - 2,
                    opcode_value,
                    opcode->mnemonic);
            } else {
                logger(LOG_DEBUG,
                    "$%04X: $%02X: %s",
                    gb->pc - 1,
                    opcode_value,
                    opcode->mnemonic);
            }

            handler_ret = (*opcode->handler)(opcode, gb);
        } else {
            handler_ret = OPCODE_ILLEGAL;
        }
    }

    if (handler_ret < 0) {
        if (handler_ret == OPCODE_ILLEGAL) {
            if (CB) {
                logger(LOG_CRIT,
                    "$%04X: CB $%02X: Illegal opcode",
                    gb->pc - 2,
                    opcode_value);
            } else {
                logger(LOG_CRIT,
                    "$%04X: $%02X: Illegal opcode",
                    gb->pc - 1,
                    opcode_value);
            }
        }
        return handler_ret;
    }

    // Exclude the current cycle from the remaining
    if (handler_ret > 0)
        handler_ret -= 1;

    gb->idle_cycles += handler_ret;
    return handler_ret;
}

void cpu_dump(gb_system_t *gb)
{
    if (gb->halt || gb->stop) printf("CPU Halted (%s)\n", gb->halt ? "HALT" : "STOP");

    printf("PC: $%04X    SP: $%04X\n", gb->pc, gb->sp);
    printf("Cycle #%zu (idle: %u)\n", gb->cycle_nb, gb->idle_cycles);

    printf("\nIME=%i\n", gb->interrupts.ime);
    printf("Bits:    7 6 5 4 3 2 1 0\n");
    printf("IE  :    ");
    for (sbyte_t i = 7; i >= 0; --i) {
        printf("%c%c",
               (gb->interrupts.ie_reg & (1 << i)) ? '1' : '0',
               i > 0 ? ' ' : '\n');
    }
    printf("IF  :    ");
    for (sbyte_t i = 7; i >= 0; --i) {
        printf("%c%c",
               (gb->interrupts.if_reg & (1 << i)) ? '1' : '0',
               i > 0 ? ' ' : '\n');
    }

    printf("\nA: $%02X    F: $%02X\n",
           gb->regs.a,
           gb->regs.f.data);
    printf("B: $%02X    C: $%02X\n",
           gb->regs.b,
           gb->regs.c);
    printf("D: $%02X    E: $%02X\n",
           gb->regs.d,
           gb->regs.e);
    printf("H: $%02X    L: $%02X\n\n",
           gb->regs.h,
           gb->regs.l);

    printf("Flags:\n");
    printf("    Z: %u\n", gb->regs.f.flags.z);
    printf("    N: %u\n", gb->regs.f.flags.n);
    printf("    H: %u\n", gb->regs.f.flags.h);
    printf("    C: %u\n", gb->regs.f.flags.c);
}