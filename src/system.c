/*
system.c
GameBoy system initialization

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
#include "gb/memory_banks.h"
#include "gb/registers.h"
#include <stdlib.h>
#include <string.h>

// Free gb_system_t
// If include_ptr is true, also free *gb
void free_gb_system(gb_system_t *gb, bool include_ptr)
{
    gb_free_membank(&gb->memory.rom_banks);
    gb_free_membank(&gb->memory.ram_banks);

    if (include_ptr)
        free(gb);
}

// Allocate and initialize gb_system_t
gb_system_t *create_gb_system(void)
{
    gb_system_t *gb = malloc(sizeof(gb_system_t));

    if (!gb)
        return NULL;

    memset(gb, 0, sizeof(gb_system_t));

    // Initialize CPU registers
    gb_register_write_u16(REG_AF, 0x01B0, gb);
    gb_register_write_u16(REG_BC, 0x0013, gb);
    gb_register_write_u16(REG_DE, 0x00D8, gb);
    gb_register_write_u16(REG_HL, 0x014D, gb);

    gb->pc = CARTRIDGE_HEADER_LADDR;
    gb->sp = HRAM_UADDR;

    // Initialize IO registers
    gb_write_byte(0xFF05, 0x00, false, gb); // TIMA
    gb_write_byte(0xFF06, 0x00, false, gb); // TMA
    gb_write_byte(0xFF07, 0x00, false, gb); // TAC
    gb_write_byte(0xFF10, 0x80, false, gb); // NR10
    gb_write_byte(0xFF11, 0xBF, false, gb); // NR11
    gb_write_byte(0xFF12, 0xF3, false, gb); // NR12
    gb_write_byte(0xFF14, 0xBF, false, gb); // NR14
    gb_write_byte(0xFF16, 0x3F, false, gb); // NR21
    gb_write_byte(0xFF17, 0x00, false, gb); // NR22
    gb_write_byte(0xFF19, 0xBF, false, gb); // NR24
    gb_write_byte(0xFF1A, 0x7F, false, gb); // NR30
    gb_write_byte(0xFF1B, 0xFF, false, gb); // NR31
    gb_write_byte(0xFF1C, 0x9F, false, gb); // NR32
    gb_write_byte(0xFF1E, 0xBF, false, gb); // NR33
    gb_write_byte(0xFF20, 0xFF, false, gb); // NR41
    gb_write_byte(0xFF21, 0x00, false, gb); // NR42
    gb_write_byte(0xFF22, 0x00, false, gb); // NR43
    gb_write_byte(0xFF23, 0xBF, false, gb); // NR30
    gb_write_byte(0xFF24, 0x77, false, gb); // NR50
    gb_write_byte(0xFF25, 0xF3, false, gb); // NR51
    gb_write_byte(0xFF26, 0xF1, false, gb); // NR52
    gb_write_byte(0xFF40, 0x91, false, gb); // LCDC
    gb_write_byte(0xFF42, 0x00, false, gb); // SCY
    gb_write_byte(0xFF43, 0x00, false, gb); // SCX
    gb_write_byte(0xFF45, 0x00, false, gb); // LYC
    gb_write_byte(0xFF47, 0xFC, false, gb); // BGP
    gb_write_byte(0xFF48, 0xFF, false, gb); // OBP0
    gb_write_byte(0xFF49, 0xFF, false, gb); // OBP1
    gb_write_byte(0xFF4A, 0x00, false, gb); // WY
    gb_write_byte(0xFF4B, 0x00, false, gb); // WX
    gb_write_byte(0xFFFF, 0x00, false, gb); // IE

    return gb;
}

// Standard GameBoy emulation loop
int emulate_gb_system(gb_system_t *gb)
{
    int last_ret;

    while ((last_ret = gb_cpu_cycle(gb, true)) >= 0) {
        continue;
    }

    return last_ret;
}