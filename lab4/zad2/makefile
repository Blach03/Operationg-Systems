CC = gcc
CFLAGS = -Wall -Wextra -pedantic

.PHONY: all clean

all: process

process: process.c
	$(CC) $(CFLAGS) -o $@ $^

clean:
	rm -f process
