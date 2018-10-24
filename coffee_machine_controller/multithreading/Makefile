CC=gcc
CFLAGS=-g -Wall -O -I. -DNDEBUG
LDFLAGS=-L.
LDLIBS=-lrt -lpthread

all: cofm

cofm: cofm.o fsm.o task.o

clean:
	$(RM) *.o *~ cofm librt.a libpthread.a
librt.a: rt.o
	ar rcs $@ $^
libpthread.a: pthread.o
	ar rcs $@ $^
