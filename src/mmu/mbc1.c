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
#include "mmu/banks.h"

#define mbc1_regs ((mbc1_regs_t *) gb->memory.mbc_regs)

// TODO: Add support for MBC1m (Multi-Game Compilation Carts)
// TODO: Test large RAM/ROM carts more

int16_t mbc1_readb(uint16_t addr, gb_system_t *gb)
{
    if (addr <= ROM_BANK_0_UADDR) {
        return membank_readb_bank(addr, mbc1_regs->large_rom_bank, &gb->memory.rom);

    } else if (ADDR_IN_RANGE(addr, ROM_BANK_N_LADDR, ROM_BANK_N_UADDR)) {
        return membank_readb(addr - ROM_BANK_N_LADDR, &gb->memory.rom);

    } else if (ADDR_IN_RANGE(addr, RAM_BANK_N_LADDR, RAM_BANK_N_UADDR)) {
        return membank_readb(addr - RAM_BANK_N_LADDR, &gb->memory.ram);

    } else {
        return -1;
    }
}

bool mbc1_writeb(uint16_t addr, byte_t value, gb_system_t *gb)
{
    if (addr <= 0x1FFF) {
        if ((mbc1_regs->ram_write_enabled = ((value & 0xF) == 0xA))) {
            logger(LOG_DEBUG, "mbc1_writeb: RAM banking enabled");
        } else {
            logger(LOG_DEBUG, "mbc1_writeb: RAM banking disabled");
        }
        return true;

    } else if (ADDR_IN_RANGE(addr, 0x2000, 0x3FFF)) {
        byte_t bank_nb = value & 0x1F;

        if (bank_nb == 0) bank_nb += 1;
        if (!mbc1_regs->ram_select)
            bank_nb |= mbc1_regs->bank_upper_bits;

        logger(LOG_DEBUG, "mbc1_writeb: Switching to ROM bank $%02X\n", bank_nb);
        membank_switch(bank_nb, &gb->memory.rom);
        return true;

    } else if (ADDR_IN_RANGE(addr, 0x4000, 0x5FFF)) {
        mbc1_regs->bank_upper_bits = (value & 0x3) << 5;
        return true;

    } else if (ADDR_IN_RANGE(addr, 0x6000, 0x7FFF)) {
        mbc1_regs->ram_select = (value & 0x1);
        if (mbc1_regs->ram_select && mbc1_regs->ram_write_enabled && mbc1_regs->large_ram_cart) {
            logger(LOG_DEBUG, "mbc1_writeb: Switching to RAM bank $%02X\n",
                (mbc1_regs->bank_upper_bits >> 5));
            membank_switch((mbc1_regs->bank_upper_bits >> 5), &gb->memory.ram);
        } else {
            mbc1_regs->large_rom_bank = mbc1_regs->bank_upper_bits;
            logger(LOG_WARN, "mbc1_writeb: Switching large ROM Bank to: $%02X\n",
                mbc1_regs->large_rom_bank);
        }
        return true;

    } else if (ADDR_IN_RANGE(addr, RAM_BANK_N_LADDR, RAM_BANK_N_UADDR)) {
        if (!mbc1_regs->ram_write_enabled) {
            logger(LOG_ERROR, "mbc1_writeb: RAM banks are disabled");
            return false;
        }
        return membank_writeb(addr - RAM_BANK_N_LADDR, value, &gb->memory.ram);

    } else {
        return false;
    }
}