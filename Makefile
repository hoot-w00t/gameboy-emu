CC	=	cc
INCLUDE	=	-Iinclude
OFLAGS	=	-g
CFLAGS	=	$(OFLAGS) -W -Wall -Wextra -pipe $(INCLUDE)
LDFLAGS	=	$(shell pkg-config --libs readline) $(shell sdl2-config --libs)

BIN_NAME	=	gameboy

SRC	=	logger.c	\
		main.c

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