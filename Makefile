# Build the image tagger program
CC = gcc

crack: crack.o sha256.o
	$(CC) -Wall -o crack crack.o sha256.o -lm

main.o: crack.c sha256.h
	$(CC) -c crack.c

sha256.o:
	$(CC) -c sha256.c
