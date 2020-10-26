/*
ppu.c
Pixel Processing Unit
Handles pixel rendering into the framebuffer

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
#include "mmu/mmu.h"

#define SHADE_FROM_PALETTE(id, palette) ((palette >> (id * 2)) & 0x3)

const pixel_t monochrome_pal[4] = {
    { .r = 0xFF, .g = 0xFF, .b = 0xFF},
    { .r = 0xBF, .g = 0xBF, .b = 0xBF},
    { .r = 0x3F, .g = 0x3F, .b = 0x3F},
    { .r = 0x00, .g = 0x00, .b = 0x00}
};

// Draw background or window on given scanline
void ppu_draw_background(const byte_t scanline, gb_system_t *gb)
{
    const byte_t scy = gb->screen.scy;
    const byte_t scx = gb->screen.scx;
    const byte_t wy = gb->screen.wy;
    const byte_t wx = gb->screen.wx - 7;
    const bool use_window = gb->screen.window_display && wy <= scanline;
    bool signed_tile_id = false;
    uint16_t base_tile_data_addr;
    uint16_t base_tile_map_addr;
    byte_t y, x;

    if (gb->screen.bg_select) {
        base_tile_data_addr = TILE_LADDR;
    } else {
        base_tile_data_addr = TILE_LADDR + 0x800;
        signed_tile_id = true;
    }

    if (use_window) {
        base_tile_map_addr = gb->screen.window_select ? BG_MAP_2_LADDR : BG_MAP_1_LADDR;
        y = scanline - wy;
    } else {
        base_tile_map_addr = gb->screen.bg_tilemap_select ? BG_MAP_2_LADDR : BG_MAP_1_LADDR;
        y = scanline + scy;
    }

    uint16_t tile_row = (y / 8) * 32;
    for (byte_t pixel = 0; pixel < SCREEN_WIDTH; ++pixel) {
        x = pixel + scx;

        if (use_window && pixel >= wx) x = pixel - wx;
        uint16_t tile_col = x / 8;
        uint16_t tile_map_addr = base_tile_map_addr + tile_row + tile_col;
        uint16_t tile_data_addr = base_tile_data_addr;

        if (signed_tile_id) {
            tile_data_addr += (((sbyte_t) mmu_readb(tile_map_addr, gb)) + 128) * 16;
        } else {
            tile_data_addr += mmu_readb(tile_map_addr, gb) * 16;
        }

        byte_t line = (y % 8) * 2;
        byte_t lo = mmu_readb(tile_data_addr + line, gb);
        byte_t hi = mmu_readb(tile_data_addr + line + 1, gb);
        byte_t pixel_bit = 7 - (x % 8);
        byte_t pixel_shade_id = (((hi >> pixel_bit) & 1) << 1) | ((lo >> pixel_bit) & 1);
        byte_t pixel_shade = SHADE_FROM_PALETTE(pixel_shade_id, gb->screen.bgp);

        if (scanline >= SCREEN_HEIGHT || pixel >= SCREEN_WIDTH) continue;
        gb->screen.framebuffer[scanline][pixel] = monochrome_pal[pixel_shade];
    }
}

// Draw scanline
void ppu_draw_scanline(const byte_t scanline, gb_system_t *gb)
{
    if (gb->screen.bg_display) ppu_draw_background(scanline, gb);
    //TODO: if (gb->screen.obj_display) ppu_draw_sprites(scanline, gb);
}

// TODO: Move DMA Transfer to CPU
// Equivalent of cpu_cycle() for the PPU
int ppu_cycle(gb_system_t *gb)
{
    byte_t old_mode;
    bool lcd_stat_int;

    if (gb->screen.dma_running > 0) {
        logger(LOG_DEBUG,
               "DMA Transfer: $%04X to $FE%02X",
               gb->screen.dma_src + gb->screen.dma_offset,
               gb->screen.dma_offset);

        gb->memory.oam[gb->screen.dma_offset] = mmu_readb(gb->screen.dma_src + gb->screen.dma_offset, gb);
        gb->screen.dma_offset += 1;
        gb->screen.dma_running -= 1;
    }

    // LCD is disabled
    if (!gb->screen.enable) return gb->screen.mode;

    old_mode = gb->screen.mode;
    lcd_stat_int = false;

    gb->screen.line_cycle += 1;
    if (gb->screen.line_cycle == LCD_MODE_2_CYCLES) {
        // Entering Draw period
        gb->screen.mode = LCDC_MODE_DRAW;

    } else if (gb->screen.line_cycle == LCD_MODE_3_CYCLES) {
        // Entering HBlank period
        if (gb->screen.hblank_int) lcd_stat_int = true;
        gb->screen.mode = LCDC_MODE_HBLANK;

    } else if (gb->screen.line_cycle >= LCD_LINE_CYCLES) {
        // One scanline completed
        gb->screen.line_cycle = 0;

        if (gb->screen.ly < 144) {
            // Draw the scanline
            ppu_draw_scanline(gb->screen.ly, gb);
            gb->screen.framebuffer_updated = true;

            gb->screen.mode = LCDC_MODE_SEARCH;
            if (gb->screen.oam_int) lcd_stat_int = true;

        } else if (gb->screen.ly == 144) {
            // VBlank period
            cpu_int_flag_set(INT_VBLANK_BIT, gb);
            gb->screen.mode = LCDC_MODE_VBLANK;
            if (gb->screen.vblank_int) lcd_stat_int = true;
        }

        // Coincidence Flag
        gb->screen.coincidence_flag = gb->screen.ly == gb->screen.lyc;
        if (gb->screen.coincidence_flag && gb->screen.coincidence_int)
            cpu_int_flag_set(INT_LCD_STAT_BIT, gb);

        gb->screen.ly += 1;
        if (gb->screen.ly >= LCD_LINES) {
            // Full frame drawn
            gb->screen.ly = 0;

            gb->screen.mode = LCDC_MODE_SEARCH;
            if (gb->screen.oam_int) lcd_stat_int = true;
        }
    }

    // LCD_STAT interrupt request if mode has changed and there is a pending IR
    if (lcd_stat_int && gb->screen.mode != old_mode)
        cpu_int_flag_set(INT_LCD_STAT_BIT, gb);

    return gb->screen.mode;
}