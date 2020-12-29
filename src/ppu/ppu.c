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
#include <string.h>

#define SHADE_FROM_PALETTE(id, palette) ((palette >> (id * 2)) & 0x3)
#define BG_MAP_2_OFFSET (BG_MAP_2_LADDR - TILE_LADDR)
#define BG_MAP_1_OFFSET (BG_MAP_1_LADDR - TILE_LADDR)

const pixel_t monochrome_pal[4] = {
    { .r = 0xFF, .g = 0xFF, .b = 0xFF},
    { .r = 0xBF, .g = 0xBF, .b = 0xBF},
    { .r = 0x3F, .g = 0x3F, .b = 0x3F},
    { .r = 0x00, .g = 0x00, .b = 0x00}
};

// Draw sprites on given scanline
void ppu_draw_sprites(const byte_t scanline, gb_system_t *gb)
{
    byte_t sprite_height = 8 + (gb->screen.lcdc.obj_size * 8);
    int16_t y, x, line;
    uint16_t tile_data_addr;
    byte_t tile_id, tile_lo, tile_hi;
    byte_t palette;
    oam_entry_t *oam_entry;

    memset(gb->screen.sl_sprite_shade_id, -1, sizeof(gb->screen.sl_sprite_shade_id));
    for (byte_t i = 0; i < gb->screen.oam_buffer_size; ++i) {
        oam_entry = gb->screen.oam_sorted[i];
        y = oam_entry->y - 16;
        x = oam_entry->x - 8;
        palette = oam_entry->attr.dmg_palette ? gb->screen.obp1 : gb->screen.obp0;

        if (oam_entry->attr.y_flip) {
            line = (-(scanline - y - (sprite_height - 1)) * 2);
        } else {
            line = ((scanline - y) * 2);
        }

        tile_id = gb->screen.lcdc.obj_size ? (oam_entry->tile_id & 0xFE) : oam_entry->tile_id;
        tile_data_addr = (tile_id * 16) + line;
        if ((unsigned) (tile_data_addr + 1) >= sizeof(gb->memory.vram)) {
            logger(LOG_CRIT, "ppu_draw_sprites: tile_data_addr out of bounds: %X", tile_data_addr);
            continue;
        }

        tile_lo = gb->memory.vram[tile_data_addr];
        tile_hi = gb->memory.vram[tile_data_addr + 1];

        for (sbyte_t pixel = 7; pixel >= 0; --pixel) {
            sbyte_t pixel_bit = oam_entry->attr.x_flip ? (-(pixel - 7)) : pixel;
            byte_t pixel_shade_id = (((tile_hi >> pixel_bit) & 1) << 1) | ((tile_lo >> pixel_bit) & 1);

            // Shade 0 is transparent for sprites
            if (pixel_shade_id == 0)
                continue;

            byte_t pixel_shade = SHADE_FROM_PALETTE(pixel_shade_id, palette);
            int16_t pixel_x = x + (7 - pixel);

            if (scanline >= SCREEN_HEIGHT || pixel_x < 0 || pixel_x >= SCREEN_WIDTH)
                continue;

            // Object pixel is behind background
            if (oam_entry->attr.obj_behind_bg && gb->screen.sl_bg_shade_id[pixel_x] != 0)
                continue;

            // Sprite overlap
            if (gb->screen.sl_sprite_shade_id[pixel_x] >= 0)
                continue;

            gb->screen.framebuffer[scanline][pixel_x] = monochrome_pal[pixel_shade];
            gb->screen.sl_sprite_shade_id[pixel_x] = pixel_shade_id;
        }
    }
}

// Draw background or window on given scanline
void ppu_draw_background(const byte_t scanline, gb_system_t *gb)
{
    const byte_t scy = gb->screen.scy;
    const byte_t scx = gb->screen.scx;
    const byte_t wy = gb->screen.wy;
    const bool use_window = gb->screen.lcdc.window_display && wy <= scanline;
    bool signed_tile_id = false;
    uint16_t base_tile_data_addr;
    uint16_t base_tile_map_addr;
    byte_t y, x, tile_map_select;

    if (gb->screen.lcdc.bg_select) {
        base_tile_data_addr = 0;
    } else {
        base_tile_data_addr = 0x800;
        signed_tile_id = true;
    }

    for (byte_t pixel = 0; pixel < SCREEN_WIDTH; ++pixel) {
        if (use_window && pixel >= (signed) (gb->screen.wx - 7)) {
            tile_map_select = gb->screen.lcdc.window_select;
            x = pixel - (gb->screen.wx - 7);
            y = scanline - wy;
        } else {
            tile_map_select = gb->screen.lcdc.bg_tilemap_select;
            x = pixel + scx;
            y = scanline + scy;
        }
        base_tile_map_addr = tile_map_select ? BG_MAP_2_OFFSET : BG_MAP_1_OFFSET;

        uint16_t tile_row = (y / 8) * 32;
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

        if (scanline >= SCREEN_HEIGHT || pixel >= SCREEN_WIDTH)
            continue;

        gb->screen.framebuffer[scanline][pixel] = monochrome_pal[pixel_shade];
        gb->screen.sl_bg_shade_id[pixel] = pixel_shade_id;
    }
}

// Draw scanline
void ppu_draw_scanline(const byte_t scanline, gb_system_t *gb)
{
#ifdef PPU_USE_PIXEL_FIFO
#error "Pixel FIFO not implemented yet"
#else
    if (gb->screen.lcdc.bg_display)
        ppu_draw_background(scanline, gb);

    if (gb->screen.lcdc.obj_display)
        ppu_draw_sprites(scanline, gb);
#endif
}

// Bubble-sort OAM buffer for sprite priority
void ppu_oam_sort(gb_system_t *gb)
{
    bool sorted = false;
    oam_entry_t *tmp;

    for (byte_t i = 0; i < gb->screen.oam_buffer_size; ++i)
        gb->screen.oam_sorted[i] = &gb->screen.oam_buffer[i];

    while (!sorted) {
        sorted = true;
        for (byte_t i = 1; i < gb->screen.oam_buffer_size; ++i) {
            if (gb->screen.oam_sorted[i - 1]->x > gb->screen.oam_sorted[i]->x) {
                tmp = gb->screen.oam_sorted[i];
                gb->screen.oam_sorted[i] = gb->screen.oam_sorted[i - 1];
                gb->screen.oam_sorted[i - 1] = tmp;
                sorted = false;
            }
        }
    }
}

// Fetch the OAM buffer (complete OAM search)
void ppu_oam_search(gb_system_t *gb)
{
    byte_t sprite_height = 8 + (gb->screen.lcdc.obj_size * 8);
    oam_entry_t *oam_entry;
    uint16_t sprite_addr;
    int16_t sprite_y_top, sprite_y_bot;

    gb->screen.oam_buffer_size = 0;
    for (byte_t sprite = 0; sprite < 40 && gb->screen.oam_buffer_size < 10; ++sprite) {
        sprite_addr = sprite * 4;
        oam_entry = (oam_entry_t *) (gb->memory.oam + sprite_addr);
        sprite_y_top = oam_entry->y - 16;
        sprite_y_bot = sprite_y_top + sprite_height;
        if (gb->screen.ly >= sprite_y_top && gb->screen.ly < sprite_y_bot) {
            gb->screen.oam_buffer[gb->screen.oam_buffer_size] = *oam_entry;
            gb->screen.oam_buffer_size += 1;
        }
    }
}

// Fetch next valid OAM entry
void ppu_oam_search_cycle(gb_system_t *gb)
{
    byte_t sprite_height = 8 + (gb->screen.lcdc.obj_size * 8);
    oam_entry_t *oam_entry;
    uint16_t sprite_addr;
    int16_t sprite_y_top, sprite_y_bot;

    if (gb->screen.oam_search_index >= 40)
        return;

    sprite_addr = gb->screen.oam_search_index * 4;
    oam_entry = (oam_entry_t *) (gb->memory.oam + sprite_addr);
    sprite_y_top = oam_entry->y - 16;
    sprite_y_bot = sprite_y_top + sprite_height;
    if (gb->screen.oam_buffer_size < 10 && gb->screen.ly >= sprite_y_top && gb->screen.ly < sprite_y_bot) {
        gb->screen.oam_buffer[gb->screen.oam_buffer_size] = *oam_entry;
        gb->screen.oam_buffer_size += 1;
    }
    gb->screen.oam_search_index += 1;
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
    if (!gb->screen.lcdc.enable)
        return gb->screen.lcd_stat.mode;

    old_mode = gb->screen.lcd_stat.mode;
    lcd_stat_int = false;

    if (gb->screen.ly < 144) {
        if (gb->screen.scanline_clock < LCD_MODE_2_CYCLES) {
            gb->screen.lcd_stat.mode = LCDC_MODE_2;
            if ((gb->screen.scanline_clock % 2) == 1) {
                ppu_oam_search_cycle(gb);
                if (gb->screen.oam_search_index >= 40)
                    ppu_oam_sort(gb);
            }
        } else if (gb->screen.scanline_clock < LCD_MODE_3_CYCLES) {
            gb->screen.lcd_stat.mode = LCDC_MODE_3;
        } else {
            gb->screen.lcd_stat.mode = LCDC_MODE_0;
        }
    }

    // One scanline completed
    if ((gb->screen.scanline_clock += 1) >= LCD_LINE_CYCLES) {
        gb->screen.scanline_clock = 0;

        if (gb->screen.ly < 144) {
            // Draw the scanline
            ppu_draw_scanline(gb->screen.ly, gb);
        }

        if ((gb->screen.ly += 1) >= LCD_LINES)
            gb->screen.ly = 0;

        // Coincidence Flag
        gb->screen.lcd_stat.coincidence_flag = gb->screen.ly == gb->screen.lyc;
        if (gb->screen.lcd_stat.coincidence_flag && gb->screen.lcd_stat.coincidence_int)
            cpu_int_flag_set(INT_LCD_STAT_BIT, gb);

        if (gb->screen.ly == 144) {
            // VBlank period
            if (gb->screen.vblank_callback)
                (*(gb->screen.vblank_callback))(gb);

            cpu_int_flag_set(INT_VBLANK_BIT, gb);
            gb->screen.lcd_stat.mode = LCDC_MODE_VBLANK;
            if (gb->screen.lcd_stat.vblank_int)
                lcd_stat_int = true;
        }
    }

    if (gb->screen.lcd_stat.mode != old_mode) {
        if (gb->screen.lcd_stat.mode == LCDC_MODE_2) {
            // Reset OAM buffer when entering OAM search
            if (gb->screen.lcd_stat.oam_int)
                lcd_stat_int = true;

            gb->screen.oam_buffer_size = 0;
            gb->screen.oam_search_index = 0;
        } else if (gb->screen.lcd_stat.mode == LCDC_MODE_0) {
            if (gb->screen.lcd_stat.hblank_int)
                lcd_stat_int = true;
        }

        // LCD_STAT interrupt request if mode has changed and there is a pending IR
        if (lcd_stat_int)
            cpu_int_flag_set(INT_LCD_STAT_BIT, gb);
    }

    return gb->screen.lcd_stat.mode;
}