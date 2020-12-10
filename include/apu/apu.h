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

double apu_generate_sample(const double atime,
                           const double amplitude,
                           gb_system_t *gb);

#define PI (3.14159265358979323846)
#define AMP_HIGH  (1.0)
#define AMP_LOW   (0.0)

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
#define apu_freq11(nrlo,nrhi) ((nrhi.freq_hi << 8) | nrlo.freq_lo)

// Calculate frequency in Hz for Tone channels 1 and 2
#define apu_tone_freq(freq_11) (131072.0 / (2048.0 - (double) freq_11))

// Generate a pulse wave sample
static inline double pulse_sample(const double atime,
                                  const double frequency,
                                  const double duty)
{
    return sin(frequency * 2 * PI * atime) > duty ? AMP_HIGH : AMP_LOW;
}

#endif