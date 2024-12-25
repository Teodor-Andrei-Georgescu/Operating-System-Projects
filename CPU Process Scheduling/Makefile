#
# "Makefile" for MLFQ
#

CC=gcc -Wall -ggdb3
FLAGS=-g -D_REENTRANT
LIBS=-pthread -lrt

all: mlfq
.PHONY: all clean

mlfq: mlfq.o queue.o
	$(CC) $(LIBS) $(FLAGS) -o mlfq mlfq.o queue.o

mlfq.o: mlfq.c queue.h
	$(CC) $(LIBS) $(FLAGS) -c mlfq.c

queue.o: queue.c queue.h
	$(CC) $(FLAGS) -c queue.c

clean:
	rm -rf *.o mlfq