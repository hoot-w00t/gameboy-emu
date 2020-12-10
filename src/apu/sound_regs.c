/*
sound_regs.c
Sound Controller Registers
Contains functions that handle reading/writing from/to the Sound Controller
registers

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
#include "apu/apu.h"

byte_t sound_reg_readb(uint16_t addr, gb_system_t *gb)
{
    switch (addr) {
        case SOUND_NR10: return (*((byte_t *) &gb->apu.regs.nr10));
        case SOUND_NR11: return (*((byte_t *) &gb->apu.regs.nr11)) & 0xC0; // Bits 7 and 6 only
        case SOUND_NR12: return (*((byte_t *) &gb->apu.regs.nr12));
        case SOUND_NR13: return 0; // Write only
        case SOUND_NR14: return (*((byte_t *) &gb->apu.regs.nr14)) & 0x40; // Bit 6 only

        case SOUND_NR21: return (*((byte_t *) &gb->apu.regs.nr21)) & 0xC0; // Bits 7 and 6 only;
        case SOUND_NR22: return (*((byte_t *) &gb->apu.regs.nr22));
        case SOUND_NR23: return 0; // Write only
        case SOUND_NR24: return (*((byte_t *) &gb->apu.regs.nr24)) & 0x40; // Bit 6 only

        case SOUND_NR30: return (*((byte_t *) &gb->apu.regs.nr30));
        case SOUND_NR31: return (*((byte_t *) &gb->apu.regs.nr31));
        case SOUND_NR32: return (*((byte_t *) &gb->apu.regs.nr32));
        case SOUND_NR33: return 0; // Write Only
        case SOUND_NR34: return (*((byte_t *) &gb->apu.regs.nr34)) & 0x40; // Bit 6 only

        case SOUND_NR41: return (*((byte_t *) &gb->apu.regs.nr41));
        case SOUND_NR42: return (*((byte_t *) &gb->apu.regs.nr42));
        case SOUND_NR43: return (*((byte_t *) &gb->apu.regs.nr43));
        case SOUND_NR44: return (*((byte_t *) &gb->apu.regs.nr44)) & 0x40; // Bit 6 only

        case SOUND_NR50: return (*((byte_t *) &gb->apu.regs.nr50));
        case SOUND_NR51: return (*((byte_t *) &gb->apu.regs.nr51));
        case SOUND_NR52: return (*((byte_t *) &gb->apu.regs.nr52));

        default:
            if (ADDR_IN_RANGE(addr, SOUND_WAVE_PATTERN_LADDR, SOUND_WAVE_PATTERN_UADDR)) {
                logger(LOG_ERROR, "sound_reg_readb: address $%04X: Wave Pattern RAM is write-only", addr);
                return 0;
            }
            logger(LOG_ERROR, "sound_reg_readb failed: unhandled address $%04X", addr);
            return 0;
    }
}

bool sound_reg_writeb(uint16_t addr, byte_t value, gb_system_t *gb)
{
    switch (addr) {
        case SOUND_NR10:
            (*((byte_t *) &gb->apu.regs.nr10)) = value;
            gb->apu.ch1.sweep = (double) gb->apu.regs.nr10.sweep_time / 128.0;
            return true;

        case SOUND_NR11:
            (*((byte_t *) &gb->apu.regs.nr11)) = value;
            switch (gb->apu.regs.nr11.wave_duty) {
                case 0x0: gb->apu.ch1.duty = 0.125; break;
                case 0x1: gb->apu.ch1.duty = 0.25;  break;
                case 0x2: gb->apu.ch1.duty = 0.50;  break;
                case 0x3: gb->apu.ch1.duty = 0.75;  break;
            }
            gb->apu.ch1.length = apu_sound_length(gb->apu.regs.nr11.sound_length);
            return true;

        case SOUND_NR12:
            (*((byte_t *) &gb->apu.regs.nr12)) = value;
            gb->apu.ch1.volume = gb->apu.regs.nr12.initial_envelope_volume;
            gb->apu.ch1.volume_step = apu_volume_step(gb->apu.regs.nr12.envelope_sweep);
            return true;

        case SOUND_NR13:
            (*((byte_t *) &gb->apu.regs.nr13)) = value;
            return true;

        case SOUND_NR14:
            (*((byte_t *) &gb->apu.regs.nr14)) = value;
            gb->apu.ch1.freq = apu_tone_freq(apu_freq11(gb->apu.regs.nr13, gb->apu.regs.nr14));
            return true;

        case SOUND_NR21:
            (*((byte_t *) &gb->apu.regs.nr21)) = value;
            switch (gb->apu.regs.nr21.wave_duty) {
                case 0x0: gb->apu.ch2.duty = 0.125; break;
                case 0x1: gb->apu.ch2.duty = 0.25;  break;
                case 0x2: gb->apu.ch2.duty = 0.50;  break;
                case 0x3: gb->apu.ch2.duty = 0.75;  break;
            }
            gb->apu.ch2.length = apu_sound_length(gb->apu.regs.nr21.sound_length);
            return true;

        case SOUND_NR22:
            (*((byte_t *) &gb->apu.regs.nr22)) = value;
            gb->apu.ch2.volume = gb->apu.regs.nr22.initial_envelope_volume;
            gb->apu.ch2.volume_step = apu_volume_step(gb->apu.regs.nr22.envelope_sweep);
            return true;

        case SOUND_NR23:
            (*((byte_t *) &gb->apu.regs.nr23)) = value;
            gb->apu.ch2.freq = apu_tone_freq(apu_freq11(gb->apu.regs.nr23, gb->apu.regs.nr24));
            return true;

        case SOUND_NR24:
            (*((byte_t *) &gb->apu.regs.nr24)) = value;
            gb->apu.ch2.freq = apu_tone_freq(apu_freq11(gb->apu.regs.nr23, gb->apu.regs.nr24));
            return true;

        case SOUND_NR30: (*((byte_t *) &gb->apu.regs.nr30)) = value; return true;
        case SOUND_NR31: (*((byte_t *) &gb->apu.regs.nr31)) = value; return true;
        case SOUND_NR32: (*((byte_t *) &gb->apu.regs.nr32)) = value; return true;
        case SOUND_NR33: (*((byte_t *) &gb->apu.regs.nr33)) = value; return true;
        case SOUND_NR34: (*((byte_t *) &gb->apu.regs.nr34)) = value; return true;

        case SOUND_NR41: (*((byte_t *) &gb->apu.regs.nr41)) = value; return true;
        case SOUND_NR42: (*((byte_t *) &gb->apu.regs.nr42)) = value; return true;
        case SOUND_NR43: (*((byte_t *) &gb->apu.regs.nr43)) = value; return true;
        case SOUND_NR44: (*((byte_t *) &gb->apu.regs.nr44)) = value; return true;

        case SOUND_NR50: (*((byte_t *) &gb->apu.regs.nr50)) = value; return true;
        case SOUND_NR51: (*((byte_t *) &gb->apu.regs.nr51)) = value; return true;
        case SOUND_NR52:
            gb->apu.regs.nr52.sound_on = (value >> 7);
            return true;

        default:
            if (ADDR_IN_RANGE(addr, SOUND_WAVE_PATTERN_LADDR, SOUND_WAVE_PATTERN_UADDR)) {
                // TODO: Emulate writing during operation (should corrupt the current Wave Pattern sample)
                if (gb->apu.regs.nr30.active) {
                    logger(LOG_ERROR, "sound_reg_writeb: address $%04X: Wave Pattern RAM is not accessible", addr);
                    return false;
                }

                (*((byte_t *) &gb->apu.regs.wave_pattern_ram[addr - SOUND_WAVE_PATTERN_LADDR])) = value;
                return true;
            }
            logger(LOG_ERROR, "sound_reg_writeb failed: unhandled address $%04X", addr);
            return false;
    }
    return true;
}