/*
serial.c
Serial Data Transfer
Contains functions that handle reading/writing from/to the LCD registers

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
#include "cpu/interrupts.h"

#define SERIAL_CLOCKS(freq) (CPU_CLOCK_SPEED / (freq))

byte_t serial_reg_readb(uint16_t addr, gb_system_t *gb)
{
    switch (addr) {
        case SERIAL_SB: return gb->serial.sb;
        case SERIAL_SC: return *((byte_t *) &gb->serial.sc) | 0x7E;
        default:
            logger(LOG_ERROR, "serial_reg_readb failed: unhandled address $%04X", addr);
            return MMU_UNMAPPED_ADDR_VALUE;
    }
}

bool serial_reg_writeb(uint16_t addr, byte_t value, gb_system_t *gb)
{
    switch (addr) {
        case SERIAL_SB:
            gb->serial.sb = value;
            return true;

        case SERIAL_SC:
            *((byte_t *) &gb->serial.sc) = value;

            if (!gb->serial.sc.transfer_start) {
                gb->serial.shift_clock = 0;
                gb->serial.shifts = 0;
            }
            if (gb->serial.sc.internal_clock) {
                gb->serial.clock_speed = SERIAL_CLOCKS(SERIAL_FREQ);
            } else if (gb->serial.plugged) {
                // A link cable is plugged
                // TODO: Retrieve the actual external clock speed from the
                //       other emulator
                gb->serial.clock_speed = SERIAL_CLOCKS(SERIAL_FREQ);
            } else {
                // No link cable, there is no external clock
                gb->serial.clock_speed = 0;
            }
            return true;

        default:
            logger(LOG_ERROR, "serial_reg_writeb failed: unhandled address $%04X", addr);
            return false;
    }
}

// TODO: Emulate link cable between two emulators
// TODO: Add an option to dump the serial transfers
// Equivalent of cpu_cycle() for the Serial Port
void serial_cycle(gb_system_t *gb)
{
    // Clock only when the transfer start flag is set to 1 and we have a
    // clock speed (internal or external)
    // If the clock speed is 0, the link cable is not plugged
    if (     gb->serial.sc.transfer_start
        &&   gb->serial.clock_speed > 0
        &&  (gb->serial.shift_clock += 1) >= gb->serial.clock_speed)
    {
        gb->serial.shift_clock = 0;
        if (gb->serial.shifts == 0) {
            // First shift initializes the in/out bytes
            if (gb->serial.plugged) {
                logger(LOG_CRIT, "Link emulation is not implemented yet");
                gb->serial.sb_in = 0xFF;
            } else {
                // When the serial port is not plugged, received bits are 1
                gb->serial.sb_in = 0xFF;
            }
        }

        gb->serial.sb <<= 1; // Local bit out
        gb->serial.sb |= (gb->serial.sb_in >> 7); // Remote bit in
        gb->serial.sb_in <<= 1; // Remote bit out

        if ((gb->serial.shifts += 1) >= 8) {
            gb->serial.shifts = 0;
            gb->serial.sc.transfer_start = 0;
            logger(LOG_DEBUG, "Serial IN: %02X", gb->serial.sb);
            cpu_int_flag_set(INT_SERIAL_BIT, gb);
        }
    }
}