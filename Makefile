CFLAGS = -Wall -Werror
LIBS = -lncurses

hello: hello_world_ncurses.o
	gcc -o hello hello_world_ncurses.o ${LIBS}

hello_world_ncurses.o: hello_world_ncurses.c
	gcc -c hello_world_ncurses.c ${CFLAGS}

clean:
	-rm menu main.o hello hello_world_ncurses.o
