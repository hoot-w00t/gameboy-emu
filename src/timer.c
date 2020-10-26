/*
timer.c
Built-in GameBoy timers

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

// TODO: Fine tune the timing and behavior of reading/writing to some of the
//       registers and timer_cycle()

byte_t timer_reg_readb(uint16_t addr, gb_system_t *gb)
{
    byte_t tmp;

    switch (addr) {
        case TIM_DIV : return gb->timer.reg_div;
        case TIM_TIMA: return gb->timer.reg_tima;
        case TIM_TMA : return gb->timer.reg_tma;
        case TIM_TAC :
            tmp = gb->timer.clock_select & 0x3;
            if (gb->timer.enable) tmp |= 0x4;
            return tmp;

        default:
            logger(LOG_ERROR, "timer_reg_readb failed: unhandled address $%04X", addr);
            return 0;
    }
}

bool timer_reg_writeb(uint16_t addr, byte_t value, gb_system_t *gb)
{
    switch (addr) {
        case TIM_DIV : gb->timer.reg_div = 0; break;
        case TIM_TIMA: gb->timer.reg_tima = value; break;
        case TIM_TMA : gb->timer.reg_tma = value; break;
        case TIM_TAC :
            gb->timer.enable = value & 0x4;
            gb->timer.clock_select = value & 0x3;
            switch (gb->timer.clock_select) {
                case 0x0: gb->timer.timer_clock = TIM_CLOCK_0; break;
                case 0x1: gb->timer.timer_clock = TIM_CLOCK_1; break;
                case 0x2: gb->timer.timer_clock = TIM_CLOCK_2; break;
                case 0x3: gb->timer.timer_clock = TIM_CLOCK_3; break;
                default: break;
            }
            break;

        default:
            logger(LOG_ERROR, "timer_reg_writeb failed: unhandled address $%04X", addr);
            return false;
    }
    return true;
}

// Emulate a timer cycle
void timer_cycle(gb_system_t *gb)
{
    gb->timer.div_cycles += 1;

    if (gb->timer.div_cycles >= TIM_CLOCK_DIV) {
        gb->timer.reg_div += 1;
        gb->timer.div_cycles = 0;
    }

    if (gb->timer.enable) {
        gb->timer.timer_cycles += 1;

        if (gb->timer.timer_cycles >= gb->timer.timer_clock) {
            if (gb->timer.reg_tima == 0xFF) {
                gb->timer.reg_tima = gb->timer.reg_tma;
                cpu_int_flag_set(INT_TIMER_BIT, gb);
            } else {
                gb->timer.reg_tima += 1;
            }

            gb->timer.timer_cycles = 0;
        }
    }
}