CC	=	cc
INCLUDE	=	-Iinclude
CFLAGS	=	-W -Wall -Wextra -pipe $(INCLUDE)
LDFLAGS	=	$(shell sdl2-config --cflags --libs)

BIN_NAME	=	gameboy

SRC	=	src/logger.c		\
		src/emulator.c		\
		src/memory.c		\
		src/memory_banks.c

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