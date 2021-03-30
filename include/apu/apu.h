/*
apu.h
Function prototypes for apu.c

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

#include "gameboy.h"
#include <math.h>

#ifndef _APU_APU_H
#define _APU_APU_H

#define PI        (3.14159265358979323846)
#define PI_HALF   (1.57079632679489661923)

// Apply the volume envelope
// vol is the 4-bit volume value
// inc is (struct sound_volume_envelope).envelope_increase
#define apu_volume_envelope(vol,inc) if ((inc) && (vol < 0xF)) { vol += 1; } else if (!(inc) && (vol > 0)) { vol -= 1; }

// Calculate volume step in seconds
#define apu_volume_step(n) (((double) n * (1.0 / 64.0)))

// Calculate the volume percentage from 4-bit volume value
#define apu_volume_percent(x) ((double) (x) / 15.0)

// Calculate the volume percentage from 3-bit volume value
#define apu_so_volume_percent(x)  ((double) (x) / 7.0)

// Calculate sound length in seconds
#define apu_sound_length(t1) ((double) (64 - t1) * (1.0 / 256.0))

// Calculate the 11-bit frequency from freq_lo and freq_hi
// nrlo is a (struct sound_freq_lo)
// nrhi is a (struct sound_freq_hi)
#define apu_freq11(nrlo,nrhi) (((nrhi.freq_hi) << 8) | (nrlo.freq_lo))

// Calculate frequency in Hz for Tone channels 1 and 2
#define apu_tone_freq(freq_11) (131072.0 / (2048.0 - (double) freq_11))

// Calculate Wave channel sound length
#define apu_wave_sound_length(t1) ((double) (256 - t1) * (1.0 / 256.0))

// Calculate Wave channel frequency
#define apu_wave_freq(freq_11) ((double) (65536 / (2048 - freq_11)))

// Calculate Wave channel sample period (duration) in seconds
#define apu_wave_period(freq) (1.0 / freq)

// Return the selected 4-bit sample
// n is the sample index (0-31)
static inline byte_t apu_wave_sample(const byte_t n,
                                     const struct sound_wave_pattern *wpram)
{
    if (n % 2) {
        return wpram[n / 2].s1;
    } else {
        return wpram[n / 2].s0;
    }
}

// Pointer to the selected wave pattern RAM byte
// gb is a pointer to gb_system_t
#define apu_wave_ram_selected(gb) ((byte_t *) &gb->apu.regs.wave_pattern_ram[gb->apu.ch3.wave_index / 2])

// Return audio sample from a 4-bit unsigned sample
#define apu_wave_audio_sample(s) (((double) (s) - 8.0) / 8.0)

static inline void ch3_update_playback_speed(gb_system_t *gb)
{
    gb->apu.ch3.freq = apu_wave_freq(apu_freq11(gb->apu.regs.nr33, gb->apu.regs.nr34));
    gb->apu.ch3.period = apu_wave_period(gb->apu.ch3.freq);
    gb->apu.ch3.wave_sample_duration = gb->apu.ch3.period / 32.0;
}

static inline void ch3_select_next_sample(gb_system_t *gb)
{
    gb->apu.ch3.wave_sample = apu_wave_sample(gb->apu.ch3.wave_index, gb->apu.regs.wave_pattern_ram);
    if ((gb->apu.ch3.wave_index += 1) >= 32)
        gb->apu.ch3.wave_index = 0;
}

#define apu_noise_r(r) ((r) ? (r) : (0.5))
#define apu_noise_freq(r,s) (524288.0 / apu_noise_r(r) / pow(2.0, (double) (s + 1)))

// Generate a pulse wave sample
static inline double pulse_sample(const double atime,
                                  const double frequency,
                                  const double duty)
{
    return sin(frequency * 2 * PI * atime) > duty ? 1.0 : -1.0;
}

void apu_lfsr_clock(gb_system_t *gb);
double apu_generate_sample(const double atime, gb_system_t *gb);
void apu_initialize(const uint32_t sample_rate, gb_system_t *gb);

#endif