CC = gcc
CFLAGS = -Wall -Wextra -pedantic

.PHONY: all clean

all: integral

integral: integral.o
	$(CC) $(CFLAGS) -o $@ $^

integral.o: integral.c
	$(CC) $(CFLAGS) -c $<

clean:
	rm -f integral integral.o
