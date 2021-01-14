/*
mbc5.c
MBC5 handler

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
#include "mmu/mbc5.h"
#include "mmu/rombanks.h"
#include "mmu/rambanks.h"

#define mbc5_regs ((mbc5_regs_t *) gb->memory.mbc_regs)
#define mbc5_switch_rom() rombank_switch_n(((mbc5_regs->rom_bank_lo | ((mbc5_regs->rom_bank_hi & 0x1) << 8)) % gb->memory.rom.banks_nb), &gb->memory.rom)

bool mbc5_writeb(uint16_t addr, byte_t value, gb_system_t *gb)
{
    if (addr <= 0x1FFF) {
        if ((gb->memory.ram.can_write = ((value & 0xF) == 0xA))) {
            logger(LOG_DEBUG, "mbc5: RAM banking enabled");
        } else {
            logger(LOG_DEBUG, "mbc5: RAM banking disabled");
        }
        gb->memory.ram.can_read = gb->memory.ram.can_write;
        return true;

    } else if (ADDR_IN_RANGE(addr, 0x2000, 0x2FFF)) {
        mbc5_regs->rom_bank_lo = value;
        mbc5_switch_rom();
        return true;

    } else if (ADDR_IN_RANGE(addr, 0x3000, 0x3FFF)) {
        mbc5_regs->rom_bank_hi = value;
        mbc5_switch_rom();
        return true;

    } else if (ADDR_IN_RANGE(addr, 0x4000, 0x5FFF)) {
        rambank_switch((value & 0x0F), &gb->memory.ram);
        return true;

    } else {
        return false;
    }
}