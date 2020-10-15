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
        case LCDC       : return gb->screen.lcdc;
        case LCDC_STATUS: return gb->screen.lcdc_status;
        case LCDC_SCY   : return gb->screen.pos.scy;
        case LCDC_SCX   : return gb->screen.pos.scx;
        case LCDC_LY    : return gb->screen.pos.ly;
        case LCDC_LYC   : return gb->screen.pos.lyc;
        case LCDC_WY    : return gb->screen.pos.wy;
        case LCDC_WX    : return gb->screen.pos.wx;
        case LCDC_BGP   : return gb->screen.gb_pal.bgp;
        case LCDC_OBP0  : return gb->screen.gb_pal.obp0;
        case LCDC_OBP1  : return gb->screen.gb_pal.obp1;
        case LCDC_DMA   : return gb->screen.dma;
        default:
            logger(LOG_ERROR, "lcd_reg_readb failed: unhandled address $%04X", addr);
            return 0;
    }
}

bool lcd_reg_writeb(uint16_t addr, byte_t value, gb_system_t *gb)
{
    switch (addr) {
        case LCDC       : gb->screen.lcdc = value;        break;
        case LCDC_STATUS: gb->screen.lcdc_status = value; break;
        case LCDC_SCY   : gb->screen.pos.scy = value;     break;
        case LCDC_SCX   : gb->screen.pos.scx = value;     break;
        case LCDC_LY    : gb->screen.pos.ly = value;      break;
        case LCDC_LYC   : gb->screen.pos.lyc = value;     break;
        case LCDC_WY    : gb->screen.pos.wy = value;      break;
        case LCDC_WX    : gb->screen.pos.wx = value;      break;
        case LCDC_BGP   : gb->screen.gb_pal.bgp = value;  break;
        case LCDC_OBP0  : gb->screen.gb_pal.obp0 = value; break;
        case LCDC_OBP1  : gb->screen.gb_pal.obp1 = value; break;
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

        default:
            logger(LOG_ERROR, "lcd_reg_writeb failed: unhandled address $%04X", addr);
            return false;
    }
    return true;
}