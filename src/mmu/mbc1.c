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

static void mbc1_update_mappings(gb_system_t *gb)
{
    byte_t switchable_bank_nb;

    switchable_bank_nb = mbc1_regs->rom_bank % gb->memory.rom.banks_nb;
    if (mbc1_regs->large_rom)
        switchable_bank_nb |= (mbc1_regs->bank_upper_bits << 5);

    rombank_switch_n(switchable_bank_nb, &gb->memory.rom);
    if (mbc1_regs->ram_select) {
        if (mbc1_regs->large_rom) {
            rombank_switch_0((mbc1_regs->bank_upper_bits << 5), &gb->memory.rom);
        }
        if (mbc1_regs->large_ram) {
            if (gb->memory.ram.can_write) {
                rambank_switch(mbc1_regs->bank_upper_bits, &gb->memory.ram);
            } else {
                rambank_switch(0, &gb->memory.ram);
            }
        }
    } else {
        rombank_switch_0(0, &gb->memory.rom);
        if (rambank_exists(&gb->memory.ram))
            rambank_switch(0, &gb->memory.ram);
    }
}

bool mbc1_writeb(uint16_t addr, byte_t value, gb_system_t *gb)
{
    switch (addr >> 12) {
        case 0x0:
        case 0x1:
            if ((gb->memory.ram.can_write = ((value & 0xF) == 0xA))) {
                logger(LOG_DEBUG, "mbc1: RAM banking enabled");
            } else {
                logger(LOG_DEBUG, "mbc1: RAM banking disabled");
            }
            gb->memory.ram.can_read = gb->memory.ram.can_write;
            break;

        case 0x2:
        case 0x3:
            mbc1_regs->rom_bank = (value & mbc1_regs->rom_mask);
            if (mbc1_regs->rom_bank == 0)
                mbc1_regs->rom_bank += 1;
            break;

        case 0x4:
        case 0x5:
            mbc1_regs->bank_upper_bits = (value & 0x3);
            break;

        case 0x6:
        case 0x7:
            mbc1_regs->ram_select = (value & 0x1);
            break;

        default: return false;
    }

    mbc1_update_mappings(gb);
    return true;
}