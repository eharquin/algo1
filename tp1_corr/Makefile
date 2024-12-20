CC=gcc
CFLAGS=-std=c99 -Wall -g
DEPS=arrays.h
OBJ=verif.o verif_func.o parse.o arrays.o

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

verif: $(OBJ)
	gcc -o $@ $^

.PHONY: clean
clean:
	rm -f $(OBJ) verif
