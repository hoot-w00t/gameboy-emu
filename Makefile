CC	=	cc

CFLAGS	=	-Wall -Wextra -Wshadow -O2 -g -pipe
CFLAGS	+=	-Iinclude $(shell sdl2-config --cflags)

LDFLAGS	=	$(shell sdl2-config --libs) -lSDL2_ttf -lm

VERSION_GIT_H	=	include/version_git.h
VERSION_GIT	=	$(strip $(shell cat $(VERSION_GIT_H) 2>/dev/null))
HEAD_COMMIT	=	$(strip $(shell git describe --always --tags --abbrev=10))

SRC	=	logger.c				\
		xalloc.c				\
		main.c					\
		emulator_utils.c			\
		emulator_events.c			\
		emulator.c				\
		cpu_view.c				\
		mmu_view.c				\
		gb_system.c				\
		cartridge.c				\
		timer.c					\
		joypad.c				\
		serial.c				\
		cpu/interrupts.c			\
		cpu/cpu.c				\
		cpu/opcodes.c				\
		cpu/opcodes/control.c			\
		cpu/opcodes/ld.c			\
		cpu/opcodes/jumps.c			\
		cpu/opcodes/calls.c			\
		cpu/opcodes/rotate.c			\
		cpu/opcodes/swap.c			\
		cpu/opcodes/shifts.c			\
		cpu/opcodes/bit.c			\
		cpu/opcodes/res.c			\
		cpu/opcodes/set.c			\
		cpu/opcodes/alu/add.c			\
		cpu/opcodes/alu/adc.c			\
		cpu/opcodes/alu/sub.c			\
		cpu/opcodes/alu/sbc.c			\
		cpu/opcodes/alu/and.c			\
		cpu/opcodes/alu/xor.c			\
		cpu/opcodes/alu/or.c			\
		cpu/opcodes/alu/cp.c			\
		cpu/opcodes/alu/inc.c			\
		cpu/opcodes/alu/dec.c			\
		mmu/rombanks.c				\
		mmu/rambanks.c				\
		mmu/mmu.c				\
		mmu/mmu_internal.c			\
		mmu/mbc1.c				\
		mmu/mbc3.c				\
		mmu/mbc5.c				\
		ppu/ppu.c				\
		ppu/lcd_regs.c				\
		apu/apu.c				\
		apu/sound_regs.c

OBJ	=	$(SRC:%.c=obj/%.o)
DEP	=	$(OBJ:.o=.d)

BIN	=	gameboy

ifdef WINDOWS
	CFLAGS	+=	-DSDL_MAIN_HANDLED
endif
ifdef WINDOWS_NOCONSOLE
	LDFLAGS	+=	-Wl,-subsystem,windows
endif

.PHONY:	all	update_version_git	clean

all:	update_version_git	$(BIN)

update_version_git:
ifneq ($(findstring $(HEAD_COMMIT), $(VERSION_GIT)), $(HEAD_COMMIT))
	@echo Updating $(VERSION_GIT_H) with commit hash $(HEAD_COMMIT)
	@echo "#define GAMEBOY_COMMIT_HASH \"$(HEAD_COMMIT)\"" > $(VERSION_GIT_H)
endif

clean:
	rm -rf obj

obj/%.o:	src/%.c
	@mkdir -p $(shell dirname $@)
	$(CC) -MMD $(CFLAGS) -o $@	-c $<

$(BIN):	$(OBJ)
	$(CC) -o $@ $^ $(LDFLAGS)

-include $(DEP)