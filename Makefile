CC	=	cc
INCLUDE	=	-Iinclude
OFLAGS	=	-g
CFLAGS	=	$(OFLAGS) -W -Wall -Wextra -pipe $(INCLUDE)
LDFLAGS	=	$(shell pkg-config --libs readline) $(shell sdl2-config --libs)

BIN_NAME	=	gameboy

SRC	=	logger.c			\
		emulator.c			\
		system.c			\
		debugger.c			\
		memory.c			\
		memory_banks.c		\
		mbc/mbc.c			\
		mbc/mbc3.c			\
		cartridge.c			\
		interrupts.c		\
		registers.c			\
		cpu.c				\
		opcodes/control.c	\
		opcodes/jumps.c		\
		opcodes/ld.c		\
		opcodes/xor.c		\
		opcodes/or.c		\
		opcodes/and.c		\
		opcodes/inc.c		\
		opcodes/dec.c		\
		opcodes/cp.c

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