# -*- Makefile -*-

CFLAGS = -Wall -Werror -std=c99 -g

main: main.o memhandler.o
	gcc main.o memhandler.o -o main 

main.o: main.c memhandler.c
	gcc $(CFLAGS) -c main.c memhandler.c 

memhandler.o: memhandler.c memhandler.h 
	gcc $(CFLAGS) -c memhandler.c memhandler.h

clean:
	rm -f *.o main