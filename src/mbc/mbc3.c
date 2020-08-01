/*
mbc3.c
GameBoy memory bank controller type 3

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
#include "gb/memory_banks.h"
#include "logger.h"

bool gb_mbc_3_readb(uint16_t address, gb_system_t *gb)
{
    if (address >= 0xA000 && address <= 0xBFFF) {
        logger(LOG_CRIT, "MBC3: To-Do: Reading from A000-BFFF (RTC Register)");
        // Only return true if RTC register is mapped, reading from the RAM banks will be handled the gb_read_byte()
        //return true;
    }

    return false;
}

bool gb_mbc_3_writeb(uint16_t address, byte_t value, gb_system_t *gb)
{
    if (address <= 0x1FFF) {
        logger(LOG_CRIT, "MBC3: To-Do: Enable/Disable RTC");

        if ((value & 0xF) == 0xA) { // RAM and Timer Enable
            gb->memory.ram_banks.enabled = true;
            // Enable writing to the RTC Registers
        } else {
            gb->memory.ram_banks.enabled = false;
            // Disable writing to the RTC Registers
        }

        return true;

    } else if (address >= 0x2000 && address <= 0x3FFF) { // ROM Bank Number
        byte_t bank_nb = value & 0b01111111;

        if (bank_nb > 0) // We use indexes so we must decrement the bank number
            bank_nb -= 1;

        gb_switch_membank(bank_nb, &gb->memory.rom_banks);
        return true;

    } else if (address >= 0x4000 && address <= 0x5FFF) { // RAM Bank Number or RTC Registers Select
        if (value <= 0x03) {
            // Switch RAM to bank number
            gb_switch_membank(value, &gb->memory.ram_banks);
        } else if (value >= 0x08 && value <= 0x0C) {
            // Map given RTC Register to 0xA000-0xBFFF
            logger(LOG_CRIT, "MBC3: To-Do: Map RTC Register to external RAM ranges");
        }

        return true;

    } else if (address >= 0x6000 && address <= 0x7FFF) { // Latch Clock Data
        logger(LOG_CRIT, "MBC3: To-Do: Latch Clock Data");

        return true;

    } else if (address >= 0xA000 && address <= 0xBFFF) {
        logger(LOG_CRIT, "MBC3: To-Do: Writing to A000-BFFF (RTC Register)");
        // Only return true if RTC register is mapped, writing to the RAM banks will be handled by gb_write_byte()
        //return true;
    }

    return false;
}