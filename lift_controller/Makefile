CC=gcc
CFLAGS=-g -Wall -O -I. -DNDEBUG
LDFLAGS=-L.
LDLIBS=-lrt

all: main

main: main.o fsm.o

clean:
	$(RM) *.o *~ main lrt.a

lrt.a: rt.o
	ar rcs $@ $^

