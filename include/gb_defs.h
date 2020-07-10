/*
gb_defs.h
Types and structures definitions

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

#include <stdint.h>
#include <stdbool.h>

#ifndef _GB_DEFS
#define _GB_DEFS

// Memory map
// LADDR is the the lower address and UADDR the upper
#define INTERRUPT_VECTOR_TABLE_LADDR (0x0000)
#define INTERRUPT_VECTOR_TABLE_UADDR (0x00FF)
#define CARTRIDGE_HEADER_LADDR       (0x0100)
#define CARTRIDGE_HEADER_UADDR       (0x014F)
#define ROM_BANK_0_LADDR             (0x0150)
#define ROM_BANK_0_UADDR             (0x3FFF)
#define ROM_BANK_N_LADDR             (0x4000)
#define ROM_BANK_N_UADDR             (0x7FFF)
#define TILE_LADDR                   (0x8000)
#define TILE_UADDR                   (0x97FF)
#define BG_MAP_1_LADDR               (0x9800)
#define BG_MAP_1_UADDR               (0x9BFF)
#define BG_MAP_2_LADDR               (0x9C00)
#define BG_MAP_2_UADDR               (0x9FFF)
#define RAM_BANK_N_LADDR             (0xA000)
#define RAM_BANK_N_UADDR             (0xBFFF)
#define RAM_BANK_0_LADDR             (0xC000)
#define RAM_BANK_0_UADDR             (0xDFFF)
#define RAM_ECHO_LADDR               (0xE000)
#define RAM_ECHO_UADDR               (0xFDFF)
#define OAM_LADDR                    (0xFE00)
#define OAM_UADDR                    (0xFE9F)
#define IO_REGISTERS_LADDR           (0xFF00)
#define IO_REGISTERS_UADDR           (0xFF7F)
#define HRAM_LADDR                   (0xFF80)
#define HRAM_UADDR                   (0xFFFE)
#define INTERRUPT_ENABLE             (0xFFFF)

// Registers index in struct gb_system
#define REG_A (0) // Accumulator
#define REG_F (1) // Flags (read-only)
#define REG_B (2)
#define REG_C (3)
#define REG_D (4)
#define REG_E (5)
#define REG_H (6)
#define REG_L (7)

#define REG_AF REG_A
#define REG_BC REG_B
#define REG_DE REG_D
#define REG_HL REG_H

// Screen and video definitions
#define GB_SCREEN_WIDTH (160)
#define GB_SCREEN_HEIGHT (144)

#define GB_MAX_SPRITES (40)
#define GB_SPRITE_WIDTH (8)
#define GB_SPRITE_MINHEIGHT (8)
#define GB_SPRITE_MAXHEIGHT (16)

// Memory definitions
#define GB_ROM_BANK_SIZE (16384)
#define GB_RAM_BANK_SIZE (8192)
#define GB_TILE_MEM_SIZE (TILE_UADDR - TILE_LADDR + 1)
#define GB_BG_MAP_SIZE (1024)
#define GB_OAM_SIZE (GB_MAX_SPRITES * 4)
#define GB_HRAM_SIZE (127)

// Type definitions
typedef uint8_t byte_t;
typedef struct gb_system gb_system_t;
typedef struct gb_memory gb_memory_t;
typedef struct gb_membank gb_membank_t;

// Structure definitions
struct gb_membank {
    byte_t **banks;       // Memory banks
    byte_t index;         // Selected memory bank index
    byte_t maxsize;       // Maximum number of memory banks
    uint16_t bank_size;   // Size in bytes of each memory bank
};

struct gb_memory {
    byte_t rom_bank_0[GB_ROM_BANK_SIZE];                // IVT and ROM bank 0
    struct gb_membank rom_banks;                        // Switchable ROM banks
    byte_t ram_bank_0[GB_RAM_BANK_SIZE];                // Internal and external RAM
    struct gb_membank ram_banks;                        // Switchable RAM banks
    byte_t vram[GB_TILE_MEM_SIZE + GB_BG_MAP_SIZE * 2]; // Video RAM (Tiles + the 2 BG Maps)
    byte_t oam[GB_OAM_SIZE];                            // Object Attribute Memory
    byte_t hram[GB_HRAM_SIZE];                          // HRAM
};

struct gb_system {
    struct gb_memory memory; // Memory areas
    byte_t registers[8];     // CPU Registers
    uint16_t pc;             // Program Counter (Initialized with CARTRIDGE_HEADER_LADDR)
    uint16_t sp;             // Stack pointer (Initialized with HRAM_UADDR)
};

#endif