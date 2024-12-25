# UVic CSC 360, Summer 2023
# Assignment #2

CC=gcc
# CFLAGS=-g -D_REENTRANT -Wall -Werror -std=c18  # Note: This version disables noisy VERBOSE messages.
CFLAGS=-g -D_REENTRANT -DVERBOSE -Wall -Werror -std=c18 # Note: This version ENABLE noisy VERBOSE messages.
HEADERS=logging.h
OBJECTS_A=logging.o kosmos-sem.o
OBJECTS_B=logging.o kosmos-mcv.o
LIBS=-lpthread

all: kosmos-sem kosmos-mcv

kosmos-sem: $(OBJECTS_A)
	$(CC) -o kosmos-sem $(OBJECTS_A) $(LIBS) $(CFLAGS)

kosmos-mcv: $(OBJECTS_B)
	$(CC) -o kosmos-mcv $(OBJECTS_B) $(LIBS) $(CFLAGS)

kosmos-sem.o: kosmos-sem.c
	$(CC) $(CFLAGS) -c kosmos-sem.c

kosmos-mcv.o: kosmos-mcv.c
	$(CC) $(CFLAGS) -c kosmos-mcv.c

logging.o: logging.c logging.h
	$(CC) $(CFLAGS) -c logging.c

clean:
	-rm -f $(OBJECTS_A) kosmos-sem $(OBJECTS_B) kosmos-mcv
