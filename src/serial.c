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

byte_t serial_reg_readb(uint16_t addr, gb_system_t *gb)
{
    byte_t tmp;

    switch (addr) {
        case SERIAL_SB: return gb->serial.sb;
        case SERIAL_SC:
            tmp = 0;
            if (gb->serial.transfer_start_flag) tmp |= 0x80;
            if (gb->serial.clock_speed) tmp |= 0x2;
            if (gb->serial.shift_clock) tmp |= 0x1;
            return tmp;

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
            gb->serial.transfer_start_flag = (value & 0x80);
            gb->serial.clock_speed = (value & 0x2);
            gb->serial.shift_clock = (value & 0x1);
            if (gb->serial.shift_clock) {
                gb->serial.shift_cycles = SERIAL_CLOCK;
            }
            return true;

        default:
            logger(LOG_ERROR, "serial_reg_writeb failed: unhandled address $%04X", addr);
            return false;
    }
}

// TODO: Implement communication between two emulators and transfer the serial
//       port data
// Equivalent of cpu_cycle() for the Serial Port
void serial_cycle(gb_system_t *gb)
{
    if (gb->serial.shift_cycles > 0) {
        gb->serial.shift_cycles -= 1;

        if (!gb->serial.shift_cycles) {
            gb->serial.sb <<= 1;

            // When the serial port is not connected, receive $FF
            gb->serial.sb |= 1;
            gb->serial.shifts += 1;

            if (gb->serial.shifts >= 8) {
                gb->serial.shifts = 0;
                cpu_int_flag_set(INT_SERIAL_BIT, gb);
            } else {
                gb->serial.shift_cycles = SERIAL_CLOCK;
            }
        }
    }
}