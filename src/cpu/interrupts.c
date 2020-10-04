/*
interrupts.c
Interrupt control, request and execution

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
#include "cpu/opcodes/calls.h"

// Return true if interrupt bit is set, false otherwise
bool cpu_int_flag(byte_t int_bit, gb_system_t *gb)
{
    return (gb->interrupts.if_reg & (1 << int_bit));
}

// Set Interrupt Flag bit
void cpu_int_flag_set(byte_t int_bit, gb_system_t *gb)
{
    gb->interrupts.if_reg |= (1 << int_bit);
}

// Clear Interrupt Flag bit
void cpu_int_flag_clear(byte_t int_bit, gb_system_t *gb)
{
    if (cpu_int_flag(int_bit, gb)) {
        gb->interrupts.if_reg ^= (1 << int_bit);
    }
}

// Execute the prioritary requested interrupt (if enabled)
// Returns 0 if no ISR is executed
// Returns ISR_CYCLES if ISR is executed
int cpu_int_isr(gb_system_t *gb)
{
    byte_t exec_bits;
    uint16_t addr;

    if (gb->interrupts.ime != IME_ENABLE) return 0;

    exec_bits = (gb->interrupts.if_reg & gb->interrupts.ie_reg);
    for (byte_t i = 0; i < 5; ++i) {
        if ((exec_bits & (1 << i))) {
            addr = INT_VBLANK + (i * 8);

            logger(LOG_DEBUG, "ISR $%02X", addr);
            gb->interrupts.ime = IME_DISABLE;
            cpu_int_flag_clear(i, gb);
            cpu_call(addr, gb);

            return ISR_CYCLES;
        }
    }

    return 0;
}