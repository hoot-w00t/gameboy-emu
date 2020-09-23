/*
gameboy.h
GameBoy and emulator types and structures definition

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
#include <stddef.h>

#ifndef _GAMEBOY_H
#define _GAMEBOY_H

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

// Register flags
#define FLAG_Z  (7)      // Zero Flag Bit
#define FLAG_N  (6)      // Add/Sub Flag Bit
#define FLAG_H  (5)      // Half Carry Flag Bit
#define FLAG_CY (4)      // Carry Flag Bit
#define FLAG_C  FLAG_CY  // Carry Flag Bit

// Screen and sprites
#define SCREEN_WIDTH (160)
#define SCREEN_HEIGHT (144)

#define MAX_SPRITES (40)
#define SPRITE_WIDTH (8)
#define SPRITE_MINHEIGHT (8)
#define SPRITE_MAXHEIGHT (16)

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
#define INTERRUPT_FLAG               (0xFF0F) // RW
#define INTERRUPT_ENABLE             (0xFFFF) // RW

struct addr_range {
    uint16_t start;
    uint16_t end;
};
#define ADDR_IN_RANGE(addr,range) ((addr) >= range.start && (addr) <= range.end)

const struct addr_range ivt_range = {.start = 0x0, .end = 0xFF};
const struct addr_range cartridge_hdr_range = {.start = 0x100, .end = 0x14F};
const struct addr_range rom_bank_0_range = {.start = 0x150, .end = 0x3FFF};
const struct addr_range rom_bank_n_range = {.start = 0x4000, .end = 0x7FFF};
const struct addr_range tile_range = {.start = 0x8000, .end = 0x97FF};
const struct addr_range bg_map_1_range = {.start = 0x9800, .end = 0x9BFF};
const struct addr_range bg_map_2_range = {.start = 0x9C00, .end = 0x9FFF};
const struct addr_range vram_range = {.start = tile_range.start, .end = bg_map_2_range.end};
const struct addr_range ram_bank_n_range = {.start = 0xA000, .end = 0xBFFF};
const struct addr_range ram_bank_0_range = {.start = 0xC000, .end = 0xDFFF};
const struct addr_range ram_echo_range = {.start = 0xE000, .end = 0xFDFF};
const struct addr_range oam_range = {.start = 0xFE00, .end = 0xFE9F};
const struct addr_range ioregs_range = {.start = 0xFF00, .end = 0xFF7F};
const struct addr_range hram_range = {.start = 0xFF80, .end = 0xFFFE};

// Memory size
#define ROM_BANK_SIZE (16384)
#define RAM_BANK_SIZE (8192)
#define TILE_MEM_SIZE (TILE_UADDR - TILE_LADDR + 1)
#define BG_MAP_SIZE (1024)
#define VRAM_SIZE (TILE_MEM_SIZE + BG_MAP_SIZE * 2)
#define OAM_SIZE (MAX_SPRITES * 4)
#define IO_REGS_SIZE (IO_REGISTERS_UADDR - IO_REGISTERS_LADDR + 1)
#define HRAM_SIZE (127)

// Cartridge header addresses
#define CR_LOGO_ADDR              (0x0104)
#define CR_TITLE_ADDR             (0x0134)
#define CR_LICENSEE_CODE_ADDR     (0x0144)
#define CR_OLD_LICENSEE_CODE_ADDR (0x014B)
#define CR_MBC_TYPE_ADDR          (0x0147)
#define CR_ROM_SIZE_ADDR          (0x0148)
#define CR_RAM_SIZE_ADDR          (0x0149)
#define CR_DEST_CODE_ADDR         (0x014A)
#define CR_ROM_VERSION_ADDR       (0x014C)
#define CR_HEADER_CHECKSUM_ADDR   (0x014D)
#define CR_GLOBAL_CHECKSUM_ADDR   (0x014E)

// Interrupts
#define INTERRUPT_VBLANK   (0x40)
#define INTERRUPT_LCD_STAT (0x48)
#define INTERRUPT_TIMER    (0x50)
#define INTERRUPT_SERIAL   (0x58)
#define INTERRUPT_JOYPAD   (0x60)

// Type definitions
typedef uint8_t byte_t;
typedef int8_t sbyte_t;
typedef struct cartridge_hdr cartridge_hdr_t;
typedef struct membank membank_t;
typedef struct mmu mmu_t;
typedef struct gb_system gb_system_t;

// Structures
struct cartridge_hdr {
    byte_t logo[48];          // Nintendo Logo Bitmap
    char title[17];           // Up to 16 characters + trailing zero
    char licensee_code[2];    // Licensee code formeat
    bool old_licensee_code;   // It is the old licensee code format
    byte_t mbc_type;          // MBC Type
    byte_t rom_banks;         // Number of ROM banks on the cartridge
    byte_t ram_banks;         // Number of RAM banks on the cartridge
    uint16_t ram_size;        // RAM bank size
    byte_t destination_code;  // Destination code
    byte_t rom_version;       // Mask ROM Version Number
    byte_t header_checksum;   // Header Checksum
    uint16_t global_checksum; // Global Checksum
};

struct membank {
    byte_t **banks;            // Memory banks
    uint16_t index;            // Index of the selected memory bank
    uint16_t max_bank_nb;      // Maximum number of memory banks
    uint16_t current_bank_nb;  // Current number of accessible memory banks
    uint16_t bank_size;        // Size in bytes of one memory bank
    bool enabled;              // Is the memory bank enabled
};

struct mmu {
    struct membank rom;          // ROM Banks
    struct membank ram;          // RAM Banks
    byte_t vram[VRAM_SIZE];      // Video RAM (Tiles + the 2 BG Maps)
    byte_t oam[OAM_SIZE];        // Object Attribute Memory
    byte_t ioregs[IO_REGS_SIZE]; // IO Registers
    byte_t hram[HRAM_SIZE];      // HRAM
};

struct gb_system {
    struct cartridge_hdr cartridge;    // Cartridge information
    struct mmu memory;                 // Memory areas
    byte_t registers[8];               // CPU Registers
    bool halt;                         // Is CPU halted
    uint16_t pc;                       // Program Counter (Initialized with CARTRIDGE_HEADER_LADDR)
    uint16_t sp;                       // Stack pointer (Initialized with HRAM_UADDR)
    uint16_t idle_cycles;              // Remaining cycles to idle (decrease at every cycle)
    size_t cycle_nb;                   // CPU Cycle #
    byte_t ime;                        // Interrupt Master Enable Flag
};

#endif