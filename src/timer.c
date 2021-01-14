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

static const uint16_t clock_divider[4] = {TIM_CLOCK_0, TIM_CLOCK_1, TIM_CLOCK_2, TIM_CLOCK_3};
#define divider_mask(div) ((div) >> 1)
#define high_to_low(initial, new, mask) (((initial) & (mask)) && !((new) & (mask)))
#define log_obscure(msg) logger(LOG_INFO, msg)

// Increment TIMA
// If TIMA overflows set tima_overflow to delay the overflow behavior
// by 4 clocks
static inline void timer_tima_inc(gb_system_t *gb)
{
    if ((gb->timer.tima += 1) == 0) {
        gb->timer.tima_overflow = 4;
    }
}

byte_t timer_reg_readb(uint16_t addr, gb_system_t *gb)
{
    switch (addr) {
        case TIM_DIV : return gb->timer.div;
        case TIM_TIMA: return gb->timer.tima;
        case TIM_TMA : return gb->timer.tma;
        case TIM_TAC : return (*((byte_t *) &gb->timer.tac));
        default:
            logger(LOG_ERROR, "timer_reg_readb failed: unhandled address $%04X", addr);
            return MMU_UNMAPPED_ADDR_VALUE;
    }
}

bool timer_reg_writeb(uint16_t addr, byte_t value, gb_system_t *gb)
{
    uint16_t old_clock;
    byte_t old_enable;

    switch (addr) {
        case TIM_DIV:
            gb->timer.div = 0;
            gb->timer.tima = 0;
            if (gb->timer.tac.enable && (gb->timer.counter & divider_mask(gb->timer.tima_clock))) {
                log_obscure("Obscure TIMA increase (write to div)");
                timer_tima_inc(gb);
            }
            break;

        case TIM_TIMA: gb->timer.tima = value; break;
        case TIM_TMA: gb->timer.tma = value; break;
        case TIM_TAC:
            old_clock = gb->timer.tima_clock;
            old_enable = gb->timer.tac.enable;
            (*((byte_t *) &gb->timer.tac)) = value;
            gb->timer.tima_clock = clock_divider[gb->timer.tac.clock_select];

            if (old_enable) {
                // No obscure TIMA increase on the DMG
            } else {
                if (gb->timer.tac.enable) {
                    if ((gb->timer.counter & divider_mask(old_clock)) && (!(gb->timer.counter & divider_mask(gb->timer.tima_clock)))) {
                        log_obscure("Obscure TIMA increase (write to tac, enable: 1>1)");
                        timer_tima_inc(gb);
                    }
                } else {
                    if ((gb->timer.counter & divider_mask(old_clock))) {
                        log_obscure("Obscure TIMA increase (write to tac, enable: 1>0)");
                        timer_tima_inc(gb);
                    }
                }
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
    uint16_t old_counter = gb->timer.counter;

    gb->timer.counter += 1;
    if (high_to_low(old_counter, gb->timer.counter, divider_mask(TIM_CLOCK_DIV)))
        gb->timer.div += 1;

    if (gb->timer.tima_overflow > 0) {
        if ((gb->timer.tima_overflow -= 1) == 0) {
            gb->timer.tima = gb->timer.tma;
            cpu_int_flag_set(INT_TIMER_BIT, gb);
        }
    }

    if (gb->timer.tac.enable && high_to_low(old_counter, gb->timer.counter, divider_mask(gb->timer.tima_clock)))
        timer_tima_inc(gb);
}