/*
lcd_regs.c
LCD Screen Registers
Contains functions that handle reading/writing from/to the LCD registers

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

byte_t lcd_reg_readb(uint16_t addr, gb_system_t *gb)
{
    switch (addr) {
        case LCDC       : return (*((byte_t *) &gb->screen.lcdc));
        case LCDC_STATUS: return (*((byte_t *) &gb->screen.lcd_stat));
        case LCDC_SCY   : return gb->screen.scy;
        case LCDC_SCX   : return gb->screen.scx;
        case LCDC_LY    : return gb->screen.ly;
        case LCDC_LYC   : return gb->screen.lyc;
        case LCDC_WY    : return gb->screen.wy;
        case LCDC_WX    : return gb->screen.wx;
        case LCDC_BGP   : return gb->screen.bgp;
        case LCDC_OBP0  : return gb->screen.obp0;
        case LCDC_OBP1  : return gb->screen.obp1;
        case LCDC_DMA   : return gb->screen.dma;
        default:
            logger(LOG_ERROR, "lcd_reg_readb failed: unhandled address $%04X", addr);
            return 0;
    }
}

bool lcd_reg_writeb(uint16_t addr, byte_t value, gb_system_t *gb)
{
    switch (addr) {
        case LCDC:
            if (gb->screen.lcdc.enable && !(value & 0x80) && gb->screen.lcd_stat.mode != LCDC_MODE_VBLANK)
                logger(LOG_CRIT, "LCD screen should only be disabled during VBlank! (currently mode %u)", gb->screen.lcd_stat.mode);

            (*((byte_t *) &gb->screen.lcdc)) = value;
            if (!gb->screen.lcdc.enable) {
                gb->screen.lcd_stat.mode = LCDC_MODE_0;
                gb->screen.ly = 0;
                gb->screen.scanline_clock = 0;
            }
            break;

        case LCDC_STATUS:
            (*((byte_t *) &gb->screen.lcd_stat)) = ((value & 0x78) | ((*((byte_t *) &gb->screen.lcd_stat)) & 0x7));
            break;

        case LCDC_DMA:
            // TODO: How should the system react to an invalid value?
            gb->screen.dma = value;

            if (value > 0xF1) {
                logger(LOG_ERROR, "lcd_reg_writeb failed: DMA value cannot exceed $F1");
            } else {
                gb->screen.dma_src = value << 8;
                gb->screen.dma_offset = 0;
                gb->screen.dma_running = LCD_DMA_CYCLES;
            }
            break;

        case LCDC_SCY   : gb->screen.scy = value;  break;
        case LCDC_SCX   : gb->screen.scx = value;  break;
        // TODO: Should LY be set to 0 when trying to write any value to LCDC_LY?
        //case LCDC_LY    : gb->screen.ly = 0; break;
        case LCDC_LYC   : gb->screen.lyc = value;  break;
        case LCDC_WY    : gb->screen.wy = value;   break;
        case LCDC_WX    : gb->screen.wx = value;   break;
        case LCDC_BGP   : gb->screen.bgp = value;  break;
        case LCDC_OBP0  : gb->screen.obp0 = value; break;
        case LCDC_OBP1  : gb->screen.obp1 = value; break;
        default:
            logger(LOG_ERROR, "lcd_reg_writeb failed: unhandled address $%04X", addr);
            return false;
    }
    return true;
}