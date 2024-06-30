CC = gcc
CFLAGS = -Wall -Wextra -pedantic -std=c99

.PHONY: all clean

all: processes

processes: processes.c
	$(CC) $(CFLAGS) -o $@ $^

clean:
	rm -f processes
