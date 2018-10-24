CC=gcc
CFLAGS=-g -Wall -O -I. -DNDEBUG
LDFLAGS=-L.
LDLIBS=-lrt

all: cofm

cofm: cofm.o fsm.o reactor.o

clean:
	$(RM) *.o *~ cofm librt.a
librt.a: rt.o
	ar rcs $@ $^
