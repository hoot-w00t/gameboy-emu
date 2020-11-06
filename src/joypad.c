/*
joypad.c
Joypad control

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

byte_t joypad_reg_readb(gb_system_t *gb)
{
    byte_t value = 0x0F;

    if (gb->joypad.select_directions) {
        if (gb->joypad.button_right) value ^= (1 << P10);
        if (gb->joypad.button_left)  value ^= (1 << P11);
        if (gb->joypad.button_up)    value ^= (1 << P12);
        if (gb->joypad.button_down)  value ^= (1 << P13);
    } else if (gb->joypad.select_buttons) {
        if (gb->joypad.button_a)      value ^= (1 << P10);
        if (gb->joypad.button_b)      value ^= (1 << P11);
        if (gb->joypad.button_select) value ^= (1 << P12);
        if (gb->joypad.button_start)  value ^= (1 << P13);
    }

    return value;
}

bool joypad_reg_writeb(const byte_t value, gb_system_t *gb)
{
    gb->joypad.select_buttons = ((value >> P15) & 1) ? false : true;
    gb->joypad.select_directions = ((value >> P14) & 1) ? false : true;

    if (gb->joypad.select_buttons && gb->joypad.select_directions) {
        logger(LOG_ERROR, "joypad_reg_writeb failed: P14 and P15 cannot both be selected");
        gb->joypad.select_buttons = false;
        gb->joypad.select_directions = false;
        return false;
    }
    return true;
}

void joypad_button(const byte_t button, const bool pressed, gb_system_t *gb)
{
    bool *state;
    bool direction = button <= BTN_LEFT ? true : false;

    switch (button) {
        case BTN_UP    : state = &gb->joypad.button_up; break;
        case BTN_DOWN  : state = &gb->joypad.button_down; break;
        case BTN_RIGHT : state = &gb->joypad.button_right; break;
        case BTN_LEFT  : state = &gb->joypad.button_left; break;
        case BTN_A     : state = &gb->joypad.button_a; break;
        case BTN_B     : state = &gb->joypad.button_b; break;
        case BTN_SELECT: state = &gb->joypad.button_select; break;
        case BTN_START : state = &gb->joypad.button_start; break;
        default : logger(LOG_ERROR, "Invalid button %u", button); return;
    }

    if (pressed && !(*state) && direction == gb->joypad.select_directions) {
        cpu_int_flag_set(INT_JOYPAD_BIT, gb);
    }
    *state = pressed;
}