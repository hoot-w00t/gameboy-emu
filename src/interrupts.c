/*
interrupts.c
GameBoy CPU Interrupts

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

#include "gb/defs.h"
#include "gb/memory.h"
#include "logger.h"

// Enable/disable the given interrupt bit in *dest
void set_interrupt_bit(byte_t interrupt, byte_t *dest, bool enable)
{
    byte_t value = enable ? 1 : 0;

    switch (interrupt) {
        case INTERRUPT_VBLANK: *dest |= value;
            break;
        case INTERRUPT_LCD_STAT: *dest |= (value << 1);
            break;
        case INTERRUPT_TIMER: *dest |= (value << 2);
            break;
        case INTERRUPT_SERIAL: *dest |= (value << 3);
            break;
        case INTERRUPT_JOYPAD: *dest |= (value << 4);
            break;
        default:
            logger(
                LOG_CRIT,
                "set_interrupt_bit: Invalid interrupt: 0x%02X",
                interrupt
            );
    }
}

// Enable/disable an interrupt request
void gb_set_interrupt_request(byte_t interrupt, bool enable, gb_system_t *gb)
{
    byte_t int_flag = gb_read_byte(INTERRUPT_FLAG, gb); // Read the current IF

    // Set  only the requested interrupt bit
    set_interrupt_bit(
        interrupt,
        &int_flag,
        enable
    );

    // Write the new IF
    gb_write_byte(INTERRUPT_FLAG, int_flag, false, gb);
}

// Enable all interrupts
void gb_enable_interrupts(gb_system_t *gb)
{
    gb->ime = gb_read_byte(INTERRUPT_ENABLE, gb);
}

// Disable all interrupts
void gb_disable_interrupts(gb_system_t *gb)
{
    gb->ime = 0;
}

// Execute an interrupt
void gb_execute_interrupt(byte_t interrupt, gb_system_t *gb)
{
    // Clear the interrupt bit
    gb_set_interrupt_request(interrupt, false, gb);

    gb_disable_interrupts(gb);

    // TODO: Call the corresponding IVT address
    //call(interrupt)
}