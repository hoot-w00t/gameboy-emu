CC	=	cc
INCLUDE	=	-Iinclude $(shell sdl2-config --cflags)
CFLAGS	=	-W -Wall -Wextra -O2 -pipe $(INCLUDE)
LDFLAGS	=	$(shell sdl2-config --libs) -lSDL2_ttf -lm

BIN_NAME	=	gameboy

SRC	=	logger.c				\
		xalloc.c				\
		main.c					\
		emulator_utils.c		\
		emulator_events.c		\
		emulator.c				\
		cpu_view.c				\
		mmu_view.c				\
		gb_system.c				\
		cartridge.c				\
		timer.c					\
		joypad.c				\
		serial.c				\
		cpu/registers.c			\
		cpu/interrupts.c		\
		cpu/cpu.c				\
		cpu/opcodes.c			\
		cpu/opcodes/control.c	\
		cpu/opcodes/ld.c		\
		cpu/opcodes/jumps.c		\
		cpu/opcodes/calls.c		\
		cpu/opcodes/rotate.c	\
		cpu/opcodes/swap.c		\
		cpu/opcodes/shifts.c	\
		cpu/opcodes/bit.c		\
		cpu/opcodes/res.c		\
		cpu/opcodes/set.c		\
		cpu/opcodes/alu/add.c	\
		cpu/opcodes/alu/adc.c	\
		cpu/opcodes/alu/sub.c	\
		cpu/opcodes/alu/sbc.c	\
		cpu/opcodes/alu/and.c	\
		cpu/opcodes/alu/xor.c	\
		cpu/opcodes/alu/or.c	\
		cpu/opcodes/alu/cp.c	\
		cpu/opcodes/alu/inc.c	\
		cpu/opcodes/alu/dec.c	\
		mmu/rombanks.c			\
		mmu/rambanks.c			\
		mmu/mmu.c				\
		mmu/mmu_internal.c		\
		mmu/mbc1.c				\
		mmu/mbc3.c				\
		ppu/ppu.c				\
		ppu/lcd_regs.c			\
		apu/apu.c				\
		apu/sound_regs.c

OBJ	=	$(SRC:%.c=obj/%.o)
DEP	=	$(OBJ:.o=.d)

ifdef DEBUG
	CFLAGS	+=	-g
endif

.PHONY:	all	clean	fclean	re

all:	$(BIN_NAME)

$(BIN_NAME):	$(OBJ)
	$(CC) -o $@ $^ $(LDFLAGS)

obj/%.o:	src/%.c
	@mkdir -p "$(shell dirname $@)"
	$(CC) -c $< -o $@ -MMD $(CFLAGS)

clean:
	rm -rf obj

fclean:	clean
	rm -f $(BIN_NAME)

re:	fclean	all

-include $(DEP)
