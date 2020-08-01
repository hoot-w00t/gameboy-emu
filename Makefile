CC	=	cc
INCLUDE	=	-Iinclude
CFLAGS	=	-W -Wall -Wextra -pipe $(INCLUDE) -g
LDFLAGS	=	$(shell sdl2-config --cflags --libs)

BIN_NAME	=	gameboy

SRC	=	src/logger.c			\
		src/emulator.c			\
		src/memory.c			\
		src/memory_banks.c		\
		src/mbc/mbc.c			\
		src/mbc/mbc3.c			\
		src/cartridge.c			\
		src/interrupts.c		\
		src/registers.c			\
		src/cpu.c				\
		src/opcodes/control.c	\
		src/opcodes/jumps.c

OBJ	=	$(SRC:.c=.o)
DEP	=	$(OBJ:.o=.d)

.PHONY:	all	clean	fclean	re

all:	$(BIN_NAME)

$(BIN_NAME):	$(OBJ)
	$(CC) -o $@ $^ $(LDFLAGS)

%.o:	%.c
	$(CC) -c $< -o $@ -MMD $(CFLAGS)

clean:
	rm -f $(OBJ) $(DEP)

fclean:	clean
	rm -f $(BIN_NAME)

re:	fclean	all

-include $(DEP)