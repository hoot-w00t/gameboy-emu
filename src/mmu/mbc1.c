/*
mbc1.c
MBC1 handler

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
#include "mmu/mbc1.h"
#include "mmu/rombanks.h"
#include "mmu/rambanks.h"

#define mbc1_regs ((mbc1_regs_t *) gb->memory.mbc_regs)

// TODO: Add support for MBC1m (Multi-Game Compilation Carts)

void mbc1_rom_switch(gb_system_t *gb)
{
    byte_t bank_nb = mbc1_regs->rom_bank;

    if (mbc1_regs->large_rom && !mbc1_regs->ram_select)
        bank_nb |= mbc1_regs->bank_upper_bits;

    rombank_switch_n(bank_nb, &gb->memory.rom);
}

void mbc1_ram_switch(gb_system_t *gb)
{
    byte_t bank_nb = 0;

    if (mbc1_regs->large_rom && !mbc1_regs->large_ram && mbc1_regs->ram_select) {
        rombank_switch_0(mbc1_regs->bank_upper_bits, &gb->memory.rom);
    } else {
        rombank_switch_0(0, &gb->memory.rom);
    }

    if (mbc1_regs->large_ram && mbc1_regs->ram_select)
        bank_nb = (mbc1_regs->bank_upper_bits >> 5);

    rambank_switch(bank_nb, &gb->memory.ram);
}

bool mbc1_writeb(uint16_t addr, byte_t value, gb_system_t *gb)
{
    if (addr <= 0x1FFF) {
        if ((gb->memory.ram.can_write = ((value & 0xF) == 0xA))) {
            logger(LOG_DEBUG, "mbc1: RAM banking enabled");
            mbc1_ram_switch(gb);
        } else {
            logger(LOG_DEBUG, "mbc1: RAM banking disabled");
        }
        gb->memory.ram.can_read = gb->memory.ram.can_write;

        return true;

    } else if (ADDR_IN_RANGE(addr, 0x2000, 0x3FFF)) {
        mbc1_regs->rom_bank = (value & mbc1_regs->rom_mask);
        if (mbc1_regs->rom_bank == 0)
            mbc1_regs->rom_bank += 1;

        mbc1_rom_switch(gb);
        return true;

    } else if (ADDR_IN_RANGE(addr, 0x4000, 0x5FFF)) {
        mbc1_regs->bank_upper_bits = (value & 0x3) << 5;
        mbc1_rom_switch(gb);
        mbc1_ram_switch(gb);
        return true;

    } else if (ADDR_IN_RANGE(addr, 0x6000, 0x7FFF)) {
        mbc1_regs->ram_select = (value & 0x1);
        mbc1_ram_switch(gb);
        return true;

    } else if (ADDR_IN_RANGE(addr, RAM_BANK_N_LADDR, RAM_BANK_N_UADDR)) {
        return false; // Let mmu_internal handle it

    } else {
        return false;
    }
}