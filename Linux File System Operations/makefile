#
# "makefile" for the file-system assignment
#

CC=gcc
CFLAGS=-c -Wall -g -DDEBUG

all: stat360fs ls360fs cat360fs stor360fs

stat360fs: stat360fs.o
	$(CC) stat360fs.o -o stat360fs

stat360fs.o: stat360fs.c disk.h
	$(CC) $(CFLAGS) stat360fs.c

ls360fs: ls360fs.o
	$(CC) ls360fs.o -o ls360fs

ls360fs.o: ls360fs.c disk.h
	$(CC) $(CFLAGS) ls360fs.c

cat360fs: cat360fs.o
	$(CC) cat360fs.o -o cat360fs

cat360fs.o: cat360fs.c disk.h
	$(CC) $(CFLAGS) cat360fs.c

stor360fs: stor360fs.o
	$(CC) stor360fs.o -o stor360fs

stor360fs.o: stor360fs.c disk.h
	$(CC) $(CFLAGS) stor360fs.c

clean:
	rm -rf *.o stat360fs ls360fs cat360fs stor360fs
