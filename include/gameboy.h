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
#define ADDR_IN_RANGE(addr,start,end) (addr >= start && addr <= end)

// LADDR is the the lower address and UADDR the upper
#define IVT_LADDR                    (0x0000)
#define IVT_UADDR                    (0x00FF)
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
#define VRAM_LADDR                   TILE_LADDR
#define VRAM_UADDR                   BG_MAP_2_UADDR
#define RAM_BANK_N_LADDR             (0xA000)
#define RAM_BANK_N_UADDR             (0xBFFF)
#define RAM_BANK_0_LADDR             (0xC000)
#define RAM_BANK_0_UADDR             (0xDFFF)
#define RAM_ECHO_LADDR               (0xE000)
#define RAM_ECHO_UADDR               (0xFDFF)
#define OAM_LADDR                    (0xFE00)
#define OAM_UADDR                    (0xFE9F)
#define IO_REGISTERS_LADDR           (0xFF00)
#define BOOTROM_REG_ADDR             (0xFF50)
#define IO_REGISTERS_UADDR           (0xFF7F)
#define HRAM_LADDR                   (0xFF80)
#define HRAM_UADDR                   (0xFFFE)
#define INTERRUPT_FLAG               (0xFF0F) // RW
#define INTERRUPT_ENABLE             (0xFFFF) // RW

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
#define CR_CGB_FLAG_ADDR          (0x0143)
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
#define ISR_CYCLES       (5)
#define IME_DISABLE      (0)
#define IME_ENABLE       (1)

#define INT_VBLANK       (0x40)
#define INT_LCD_STAT     (0x48)
#define INT_TIMER        (0x50)
#define INT_SERIAL       (0x58)
#define INT_JOYPAD       (0x60)

#define INT_VBLANK_BIT   (0)
#define INT_LCD_STAT_BIT (1)
#define INT_TIMER_BIT    (2)
#define INT_SERIAL_BIT   (3)
#define INT_JOYPAD_BIT   (4)

// Timer definitions
#define CPU_CLOCK_SPEED (4194304)                // CPU Clock Speed in Hz
#define TIM_DIV         (0xFF04)                 // Timer Divider (RW)
#define TIM_TIMA        (0xFF05)                 // Timer Counter (RW)
#define TIM_TMA         (0xFF06)                 // Timer Modulo (RW)
#define TIM_TAC         (0xFF07)                 // Timer Control (RW)
#define TIM_CLOCK_0     (CPU_CLOCK_SPEED / 1024) // TAC Clock Select 0 (4096)
#define TIM_CLOCK_1     (CPU_CLOCK_SPEED / 16)   // TAC Clock Select 1 (262144)
#define TIM_CLOCK_2     (CPU_CLOCK_SPEED / 64)   // TAC Clock Select 2 (65536)
#define TIM_CLOCK_3     (CPU_CLOCK_SPEED / 256)  // TAC Clock Select 3 (16384)
#define TIM_CLOCK_DIV   (16384)                  // Timer Divider Clock
// LCD IO Registers
#define LCDC         (0xFF40) // LCD Control Register
#define LCDC_STATUS  (0xFF41) // LCD Status Register

// Positions and scrolling
#define LCDC_SCY     (0xFF42) // Scroll Y (RW)
#define LCDC_SCX     (0xFF43) // Scroll X (RW)
#define LCDC_LY      (0xFF44) // Y-Coordinate (RO)
#define LCDC_LYC     (0xFF45) // LY Compare (RW)
#define LCDC_WY      (0xFF4A) // Window Y Position (RW)
#define LCDC_WX      (0xFF4B) // Window X Position -7 (RW)

// Monochrome Palettes
#define LCDC_BGP     (0xFF47) // BG Palette Data (RW)
#define LCDC_OBP0    (0xFF48) // Object Palette 0 Data (RW)
#define LCDC_OBP1    (0xFF49) // Object Palette 1 Data (RW)

// OAM DMA Transfers
#define LCDC_DMA     (0xFF46) // DMA Transfer

// VRAM Modes
#define LCDC_MODE_0           (0)         // H-Blank Period
#define LCDC_MODE_HBLANK      LCDC_MODE_0
#define LCDC_MODE_1           (1)         // V-Blank Period
#define LCDC_MODE_VBLANK      LCDC_MODE_1
#define LCDC_MODE_2           (2)         // Searching objects
#define LCDC_MODE_SEARCH      LCDC_MODE_2
#define LCDC_MODE_3           (3)         // Drawing
#define LCDC_MODE_DRAW        LCDC_MODE_3

// LCD Definitions
#define LCD_LINES                       (154)
#define LCD_DMA_CYCLES                  (160)
#define LCD_MODE_2_CYCLES               (80)
#define LCD_MODE_3_CYCLES               (172) // Shortest mode 3
#define LCD_LINE_CYCLES                 (456)
#define LCD_FRAME_CYCLES                (LCD_LINES * LCD_LINE_CYCLES)

// Video Monochrome shades
#define GB_PALETTE_WHITE      (0)
#define GB_PALETTE_LIGHT_GRAY (1)
#define GB_PALETTE_DARK_GRAY  (2)
#define GB_PALETTE_BLACK      (3)

// Tile definitions
#define TILE_SIZE (16)

// Serial definitions
#define SERIAL_SB    (0xFF01)
#define SERIAL_SC    (0xFF02)
#define SERIAL_CLOCK (8192) // In Hz

// Joypad definitions
#define JOYPAD_REG (0xFF00)
#define P10 (0) // Input Right or Button A
#define P11 (1) // Input Left  or Button B
#define P12 (2) // Input Up    or Select
#define P13 (3) // Input Down  or Start
#define P14 (4) // Select directions
#define P15 (5) // Select buttons
#define BTN_UP     (0)
#define BTN_DOWN   (1)
#define BTN_RIGHT  (2)
#define BTN_LEFT   (3)
#define BTN_A      (4)
#define BTN_B      (5)
#define BTN_SELECT (6)
#define BTN_START  (7)

// Return values for the opcode handlers
#define OPCODE_ILLEGAL   (-1)  // Illegal opcode
#define OPCODE_EXIT      (-2)  // Break out of the CPU loop

// Type definitions
typedef uint8_t byte_t;
typedef int8_t sbyte_t;
typedef struct pixel pixel_t;
typedef struct lcd_screen lcd_screen_t;
typedef struct cartridge_hdr cartridge_hdr_t;
typedef struct membank membank_t;
typedef struct mmu mmu_t;
typedef struct gb_system gb_system_t;
typedef byte_t (*mmu_readb_t)(uint16_t, gb_system_t *);
typedef bool (*mmu_writeb_t)(uint16_t, byte_t, gb_system_t *);
typedef struct opcode opcode_t;
typedef int (*opcode_handler_t)(const opcode_t *, gb_system_t *);

// Structures
struct pixel {
    byte_t r;
    byte_t g;
    byte_t b;
};

struct lcd_screen {
    // LCD Control Register (RW)
    bool bg_display;        // Bit 0 -- Background/Window display
    bool obj_display;       // Bit 1 -- Object display (sprites)
    bool obj_size;          // Bit 2 -- Object size (8x8 or 8x16 tiles)
    bool bg_tilemap_select; // Bit 3 -- Background Tile Map Select
    bool bg_select;         // Bit 4 -- Background and Window Tile Data Select
    bool window_display;    // Bit 5 -- Window Display
    bool window_select;     // Bit 6 -- Window Tile Map Select
    bool enable;            // Bit 7 -- LCD Enable

    // LCD Status Register
    byte_t mode;            // Bits 0-1 -- LCD Mode (RO)
    bool coincidence_flag;  // Bit 2    -- Coincidence Flag (RO)
    bool hblank_int;        // Bit 3    -- H-Blank Interrupt (RW)
    bool vblank_int;        // Bit 4    -- V-Blank Interrupt (RW)
    bool oam_int;           // Bit 5    -- OAM Interrupt (RW)
    bool coincidence_int;   // Bit 6    -- Coincidence Interrupt (RW)

    // LCD Positions and Scrolling Registers
    byte_t scy; // Scroll Y (RW)
    byte_t scx; // Scroll X (RW)
    byte_t ly;  // Current scanline (RO)
    byte_t lyc; // Value to compare with LY (RW)
    byte_t wy;  // Window Y position (RW)
    byte_t wx;  // Window X position minus 7 (RW)

    // LCD Monochrome Palettes (RW)
    byte_t bgp;     // BG Palette Data
    byte_t obp0;    // Object Palette 0 Data
    byte_t obp1;    // Object Palette 1 Data

    // DMA
    byte_t dma;                     // DMA Transfer
    uint16_t dma_src;               // DMA Source Address
    byte_t dma_offset;              // DMA Address Offset
    byte_t dma_running;             // DMA Transfer Remaining Cycles

    // Screen framebuffer to hold the pixels
    pixel_t framebuffer[SCREEN_HEIGHT][SCREEN_WIDTH];
    bool framebuffer_updated; // Set to true when framebuffer was updated

    // LCD State
    uint32_t line_cycle;
};

struct serial_port {
    byte_t sb;

    // Serial Transfer Control Register (SC)
    bool transfer_start_flag; // Bit 7
    bool clock_speed;         // Bit 1 (unused on the DMG)
    bool shift_clock;         // Bit 0
                              // 0 == External Clock
                              // 1 == Internal Clock

    byte_t shifts;            // # of shifts left
    uint32_t shift_cycles;    // # of cycles before shifting a bit
};

struct joypad {
    bool select_buttons;
    bool select_directions;

    // Buttons (true == pressed, false == not pressed)
    bool button_up;
    bool button_down;
    bool button_right;
    bool button_left;
    bool button_a;
    bool button_b;
    bool button_start;
    bool button_select;
};

struct cartridge_hdr {
    byte_t logo[48];          // Nintendo Logo Bitmap
    char title[17];           // Up to 16 characters + trailing zero
    char licensee_code[2];    // Licensee code formeat
    bool old_licensee_code;   // It is the old licensee code format
    byte_t mbc_type;          // MBC Type
    uint16_t rom_banks;       // Number of ROM banks on the cartridge
    uint16_t ram_banks;       // Number of RAM banks on the cartridge
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
    uint16_t bank0_size;       // Size in bytes of the memory bank #0
    uint16_t bank_size;        // Size in bytes of the other memory banks
    bool enabled;              // Is the memory bank enabled
};

struct mmu {
    byte_t bootrom_reg;          // Register $FF50
    struct membank rom;          // ROM Banks
    struct membank ram;          // RAM Banks
    byte_t vram[VRAM_SIZE];      // Video RAM (Tiles + the 2 BG Maps)
    byte_t oam[OAM_SIZE];        // Object Attribute Memory
    byte_t ioregs[IO_REGS_SIZE]; // IO Registers
    byte_t hram[HRAM_SIZE];      // HRAM
    mmu_readb_t readb_f;         // mbc_readb function pointer
    mmu_writeb_t writeb_f;       // mbc_writeb function pointer
};

struct interrupts {
    byte_t ime;     // Interrupt Master Enable flag
    byte_t ie_reg;  // Interrupt Flag register
    byte_t if_reg;  // Interrupt Enable register
};

struct builtin_timer {
    byte_t reg_div;        // Timer Divider
    byte_t reg_tima;       // Timer Counter
    byte_t reg_tma;        // Timer Modulo

    // TAC register
    bool enable;           // Timer Enable (bit 2)
    byte_t clock_select;   // Input Clock Select (bits 0-1)

    uint16_t div_cycles;   // Divider Cycles
    uint32_t timer_clock;  // Input Clock Select frequency
    uint32_t timer_cycles; // Timer Cycles
};

struct gb_system {
    struct lcd_screen screen;          // GameBoy Video Screen
    struct cartridge_hdr cartridge;    // Cartridge information
    struct mmu memory;                 // Memory areas
    struct interrupts interrupts;      // Interrupt registers
    struct builtin_timer timer;        // Built-in GameBoy timer
    struct joypad joypad;              // Joypad
    struct serial_port serial;         // Serial Port
    byte_t registers[8];               // CPU Registers
    bool halt;                         // HALT (CPU halted until interrupt)
    bool stop;                         // STOP (CPU and LCD halted until button press)
    uint16_t pc;                       // Program Counter (Initialized with CARTRIDGE_HEADER_LADDR)
    uint16_t sp;                       // Stack pointer (Initialized with HRAM_UADDR)
    uint16_t idle_cycles;              // Remaining cycles to idle (decrease at every cycle)
    size_t cycle_nb;                   // CPU Cycle #
};

struct opcode {
    char *mnemonic;            // Mnemonic
    byte_t opcode;             // Opcode
    byte_t length;             // Instruction length in bytes
    byte_t cycles_true;        // Opcode duration in cycles (if conditionnal, only if condition is true)
    byte_t cycles_false;       // Opcode duration in cycles (if conditionnal and condition if false)
    char *comment;             // What the instruction does
    opcode_handler_t handler;  // Function pointer to the opcode handler
};

#endif