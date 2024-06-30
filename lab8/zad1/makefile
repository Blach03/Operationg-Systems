CC = gcc
CFLAGS = -Wall -Wextra -std=c99

SOURCES = init.c user.c printer.c
HEADERS = common.h

TARGETS = init user printer

all: $(TARGETS)

init: init.c $(HEADERS)
	$(CC) $(CFLAGS) -o $@ init.c

user: user.c $(HEADERS)
	$(CC) $(CFLAGS) -o $@ user.c

printer: printer.c $(HEADERS)
	$(CC) $(CFLAGS) -o $@ printer.c

clean:
	rm -f $(TARGETS) *.o

.PHONY: all clean
