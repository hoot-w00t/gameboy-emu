CC	=	cc
INCLUDE	=	-Iinclude $(shell sdl2-config --cflags)
OFLAGS	=	-g
CFLAGS	=	$(OFLAGS) -W -Wall -Wextra -pipe $(INCLUDE)
LDFLAGS	=	$(shell pkg-config --libs readline) $(shell sdl2-config --libs)

BIN_NAME	=	gameboy

SRC	=	logger.c				\
		xalloc.c				\
		main.c					\
		emulator.c				\
		gb_system.c				\
		cartridge.c				\
		cpu/registers.c			\
		cpu/interrupts.c		\
		cpu/cpu.c				\
		cpu/opcodes.c			\
		cpu/opcodes/control.c	\
		cpu/opcodes/ld.c		\
		cpu/opcodes/jumps.c		\
		cpu/opcodes/calls.c		\
		mmu/banks.c				\
		mmu/mmu.c				\
		mmu/mbc0.c

OBJ	=	$(SRC:%.c=obj/%.o)
DEP	=	$(OBJ:.o=.d)

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