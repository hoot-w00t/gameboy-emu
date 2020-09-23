/*
emulator.c
GameBoy emulator

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

#include "gb_system.h"
#include <SDL.h>

// GameBoy emulation loop
int gb_system_emulate(gb_system_t *gb)
{
    printf("Loaded: %s\n", gb->cartridge.title);
    return 0;
}