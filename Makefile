# -*- Makefile -*-

CFLAGS = -Wall -Werror -std=c99 -g

testMemHandler: testMemHandler.o
	gcc -o testMemHandler testMemHandler.o $(CFLAGS) -L. -lmemhandler -pthread -Wl,-rpath=.

testMemHandler.o: testMemHandler.c
	gcc -c testMemHandler.c $(CFLAGS) -L. -lmemhandler -pthread -Wl,-rpath=. 

test: testMemHandler
	./testMemHandler $(ARGS) 	

clean:
	rm -f *.o testMemHandler