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

// Draw sprites on given scanline
void ppu_draw_sprites(const byte_t scanline, gb_system_t *gb)
{
    byte_t sprite_addr, y, x, tile_id, attributes, size, palette;
    bool y_flip, x_flip, palette_nb;
    uint16_t tile_data_addr;
    sbyte_t line;
    byte_t lo, hi;

    for (byte_t sprite = 0; sprite < 40; ++sprite) {
        sprite_addr = sprite * 4;
        y = gb->memory.oam[sprite_addr] - 16;
        x = gb->memory.oam[sprite_addr + 1] - 8;
        tile_id = gb->memory.oam[sprite_addr + 2];
        attributes = gb->memory.oam[sprite_addr + 3];
        size = gb->screen.obj_size ? 16 : 8;
        y_flip = (attributes >> 6) & 1;
        x_flip = (attributes >> 5) & 1;
        palette_nb = (attributes >> 4) & 1;
        palette = palette_nb ? gb->screen.obp1 : gb->screen.obp0;

        if (scanline >= y && scanline < y + size) {
            line = y_flip ? (-(scanline - y - size) * 2) : ((scanline - y) * 2);

            tile_data_addr = (tile_id * 16) + line;
            lo = gb->memory.vram[tile_data_addr];
            hi = gb->memory.vram[tile_data_addr + 1];

            for (sbyte_t pixel = 7; pixel >= 0; --pixel) {
                sbyte_t pixel_bit = x_flip ? (-(pixel - 7)) : pixel;
                byte_t pixel_shade_id = (((hi >> pixel_bit) & 1) << 1) | ((lo >> pixel_bit) & 1);

                // Shade 0 is transparent for sprites
                if (pixel_shade_id == 0) continue;

                byte_t pixel_shade = SHADE_FROM_PALETTE(pixel_shade_id, palette);
                int16_t pixel_x = x + (7 - pixel);

                if (scanline >= SCREEN_HEIGHT || pixel_x < 0 || pixel_x >= SCREEN_WIDTH)
                    continue;

                gb->screen.framebuffer[scanline][pixel_x] = monochrome_pal[pixel_shade];
            }
        }
    }
}

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
        base_tile_data_addr = 0;
    } else {
        base_tile_data_addr = 0x800;
        signed_tile_id = true;
    }

    if (use_window) {
        base_tile_map_addr = gb->screen.window_select ? BG_MAP_2_LADDR - TILE_LADDR : BG_MAP_1_LADDR - TILE_LADDR;
        y = scanline - wy;
    } else {
        base_tile_map_addr = gb->screen.bg_tilemap_select ? BG_MAP_2_LADDR - TILE_LADDR : BG_MAP_1_LADDR - TILE_LADDR;
        y = scanline + scy;
    }

    uint16_t tile_row = (y / 8) * 32;
    for (byte_t pixel = 0; pixel < SCREEN_WIDTH; ++pixel) {
        x = use_window ? pixel - wx : pixel + scx;

        uint16_t tile_col = x / 8;
        uint16_t tile_map_addr = base_tile_map_addr + tile_row + tile_col;
        uint16_t tile_data_addr = base_tile_data_addr;

        if (signed_tile_id) {
            tile_data_addr += (((sbyte_t) gb->memory.vram[tile_map_addr]) + 128) * 16;
        } else {
            tile_data_addr += gb->memory.vram[tile_map_addr] * 16;
        }

        byte_t line = (y % 8) * 2;
        byte_t lo = gb->memory.vram[tile_data_addr + line];
        byte_t hi = gb->memory.vram[tile_data_addr + line + 1];;
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
    if (gb->screen.obj_display) ppu_draw_sprites(scanline, gb);
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