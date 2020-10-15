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
#include "mmu/mmu.h"

// Equivalent of cpu_cycle() for the PPU
int ppu_cycle(gb_system_t *gb)
{
    if (gb->screen.dma_running > 0) {
        logger(LOG_DEBUG,
               "DMA Transfer: $%04X to $FE%02X",
               gb->screen.dma_src + gb->screen.dma_offset,
               gb->screen.dma_offset);

        gb->memory.oam[gb->screen.dma_offset] = mmu_readb(gb->screen.dma_src + gb->screen.dma_offset, gb);
        gb->screen.dma_offset += 1;
        gb->screen.dma_running -= 1;
    }

    if (gb->screen.frame_cycle == LCD_FRAME_CYCLES) gb->screen.frame_cycle = 0;
    else gb->screen.frame_cycle += 1;

    return 0;
}