# GameBoy Emulator
![GitHub](https://img.shields.io/github/license/hoot-w00t/gameboy-emu?style=flat-square)

GameBoy Emulator made in C using the SDL2 library

## Compiling from source
This project requires the `SDL2` and `SDL2_ttf` libraries.
It can be compiled for Linux or Windows (using MinGW).

```
git clone https://github.com/hoot-w00t/gameboy-emu.git
cd gameboy-emu
make
```

## Usage
Emulate a ROM
```
$ ./gameboy path_to_rom.gb
```

You can also run it without a ROM, it will prompt you to drag and drop one.