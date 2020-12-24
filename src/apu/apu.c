/*
apu.c
Audio Processing Unit
Generate audio samples to reproduce the GameBoy's APU

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

#include "apu/apu.h"

static inline double ch1_sample(const double atime, gb_system_t *gb)
{
    if (gb->apu.regs.nr14.initial) {
        gb->apu.regs.nr14.initial = 0;
        gb->apu.regs.nr52.ch1_on = 1;
        gb->apu.ch1.stop_at = atime + gb->apu.ch1.length;
        gb->apu.ch1.next_volume_step = atime + gb->apu.ch1.volume_step;
        gb->apu.ch1.next_sweep = atime + gb->apu.ch1.sweep;
        gb->apu.ch1.freq11 = apu_freq11(gb->apu.regs.nr13, gb->apu.regs.nr14);
    }
    if (!gb->apu.regs.nr52.ch1_on || (gb->apu.regs.nr14.counter_select && atime >= gb->apu.ch1.stop_at)) {
        gb->apu.regs.nr52.ch1_on = 0;
        return 0.0;
    }

    if (gb->apu.ch1.sweep > 0.0 && atime >= gb->apu.ch1.next_sweep) {
        if (gb->apu.regs.nr10.sweep_decrease) {
            gb->apu.ch1.freq11 -= (gb->apu.ch1.freq11 >> gb->apu.regs.nr10.sweep_shift);
        } else {
            gb->apu.ch1.freq11 += (gb->apu.ch1.freq11 >> gb->apu.regs.nr10.sweep_shift);
        }

        if (gb->apu.ch1.freq11 > 2047) {
            gb->apu.regs.nr52.ch1_on = 0;
            return 0.0;
        }

        gb->apu.regs.nr13.freq_lo = (gb->apu.ch1.freq11 & 0xFF);
        gb->apu.regs.nr14.freq_hi = ((gb->apu.ch1.freq11 >> 8) & 0x7);
        gb->apu.ch1.freq = apu_tone_freq(gb->apu.ch1.freq11);
        gb->apu.ch1.next_sweep = atime + gb->apu.ch1.sweep;
    }

    if (gb->apu.ch1.volume_step && atime >= gb->apu.ch1.next_volume_step) {
        apu_volume_envelope(gb->apu.ch1.volume, gb->apu.regs.nr12.envelope_increase)
        gb->apu.ch1.next_volume_step = atime + gb->apu.ch1.volume_step;
    }

    return apu_volume_percent(gb->apu.ch1.volume) * pulse_sample(atime, gb->apu.ch1.freq, gb->apu.ch1.duty);
}

static inline double ch2_sample(const double atime, gb_system_t *gb)
{
    if (gb->apu.regs.nr24.initial) {
        gb->apu.regs.nr24.initial = 0;
        gb->apu.regs.nr52.ch2_on = 1;
        gb->apu.ch2.stop_at = atime + gb->apu.ch2.length;
        gb->apu.ch2.next_volume_step = atime + gb->apu.ch2.volume_step;
    }
    if (!gb->apu.regs.nr52.ch2_on || (gb->apu.regs.nr24.counter_select && atime >= gb->apu.ch2.stop_at)) {
        gb->apu.regs.nr52.ch2_on = 0;
        return 0.0;
    }

    if (gb->apu.ch2.volume_step && atime >= gb->apu.ch2.next_volume_step) {
        apu_volume_envelope(gb->apu.ch2.volume, gb->apu.regs.nr22.envelope_increase)
        gb->apu.ch2.next_volume_step = atime + gb->apu.ch2.volume_step;
    }

    return apu_volume_percent(gb->apu.ch2.volume) * pulse_sample(atime, gb->apu.ch2.freq, gb->apu.ch2.duty);
}

static inline double ch3_sample(const double atime, gb_system_t *gb)
{
    if (gb->apu.regs.nr34.initial) {
        gb->apu.regs.nr34.initial = 0;
        gb->apu.regs.nr52.ch3_on = 1;
        gb->apu.ch3.stop_at = atime + gb->apu.ch3.length;
        gb->apu.ch3.wave_index = 0;
        gb->apu.ch3.time_elapsed = 0;
    }

    if (!gb->apu.regs.nr30.active || !gb->apu.regs.nr52.ch3_on || (gb->apu.regs.nr34.counter_select && atime >= gb->apu.ch3.stop_at)) {
        gb->apu.regs.nr52.ch3_on = 0;
        return 0.0;
    }

    if (gb->apu.ch3.wave_sample_duration <= 0.0)
        return 0.0;

    gb->apu.ch3.time_elapsed += gb->apu.sample_duration;
    while (gb->apu.ch3.time_elapsed >= gb->apu.ch3.wave_sample_duration) {
        ch3_select_next_sample(gb);
        gb->apu.ch3.time_elapsed -= gb->apu.ch3.wave_sample_duration;
    }

    if (gb->apu.regs.nr32.output_level > 0) {
        gb->apu.ch3.sample_out = (gb->apu.ch3.wave_sample >> (gb->apu.regs.nr32.output_level - 1));
    } else {
        gb->apu.ch3.sample_out = 0;
    }
    return apu_wave_audio_sample(gb->apu.ch3.sample_out) * AMP_HIGH;
}

static inline double ch4_sample(const double atime, gb_system_t *gb)
{
    if (gb->apu.regs.nr44.initial) {
        gb->apu.regs.nr44.initial = 0;
        gb->apu.regs.nr52.ch4_on = 1;
        gb->apu.ch4.stop_at = atime + gb->apu.ch4.length;
        gb->apu.ch4.next_volume_step = atime + gb->apu.ch4.volume_step;
    }
    if (!gb->apu.regs.nr52.ch4_on || (gb->apu.regs.nr44.counter_select && atime >= gb->apu.ch4.stop_at)) {
        gb->apu.regs.nr52.ch4_on = 0;
        return 0.0;
    }

    if (gb->apu.ch4.volume_step && atime >= gb->apu.ch4.next_volume_step) {
        apu_volume_envelope(gb->apu.ch4.volume, gb->apu.regs.nr42.envelope_increase)
        gb->apu.ch4.next_volume_step = atime + gb->apu.ch4.volume_step;
    }

    if ((gb->apu.lfsr & 0x1))
        return 0.0;
    return apu_volume_percent(gb->apu.ch4.volume) * AMP_HIGH;
}

void apu_lfsr_clock(gb_system_t *gb)
{
    byte_t lfsr_xor;

    // XOR bits 1-0 and shift lfsr to the right
    lfsr_xor = (gb->apu.lfsr & 0x1);
    gb->apu.lfsr >>= 1;
    lfsr_xor ^= (gb->apu.lfsr & 0x1);

    // Put XOR result on bit 15
    gb->apu.lfsr |= (lfsr_xor << 14);
    if (gb->apu.regs.nr43.counter_width) {
        // Put XOR result on bit 7 when counter_width is set to 7 bits
        gb->apu.lfsr &= ~(1 << 6);
        gb->apu.lfsr |= (lfsr_xor << 6);
    }
}

double apu_generate_sample(const double atime,
                           const double amplitude,
                           gb_system_t *gb)
{
    double sample = 0.0;
    double so1_volume = apu_so_volume_percent(gb->apu.regs.nr50.so1_volume);
    double so2_volume = apu_so_volume_percent(gb->apu.regs.nr50.so2_volume);

    if (!gb->apu.regs.nr52.sound_on)
        return 0.0;

    if (gb->apu.regs.nr51.ch1_to_so1) {
        sample += so1_volume * ch1_sample(atime, gb);
    } else if (gb->apu.regs.nr51.ch1_to_so2) {
        sample += so2_volume * ch1_sample(atime, gb);
    }

    if (gb->apu.regs.nr51.ch2_to_so1) {
        sample += so1_volume * ch2_sample(atime, gb);
    } else if (gb->apu.regs.nr51.ch2_to_so2) {
        sample += so2_volume * ch2_sample(atime, gb);
    }

    if (gb->apu.regs.nr51.ch3_to_so1) {
        sample += so1_volume * ch3_sample(atime, gb);
    } else if (gb->apu.regs.nr51.ch3_to_so2) {
        sample += so2_volume * ch3_sample(atime, gb);
    }

    if (gb->apu.regs.nr51.ch4_to_so1) {
        sample += so1_volume * ch4_sample(atime, gb);
    } else if (gb->apu.regs.nr51.ch4_to_so2) {
        sample += so2_volume * ch4_sample(atime, gb);
    }

    return sample * amplitude;
}

void apu_initialize(const uint32_t sample_rate, gb_system_t *gb)
{
    gb->apu.sample_rate = sample_rate;
    gb->apu.sample_duration = 1.0 / (double) sample_rate;
}