CC = gcc
CFLAGS = -Wall -Wextra -pthread

all: server client

server: server.c
	$(CC) $(CFLAGS) -o $@ $^ -lrt

client: client.c
	$(CC) $(CFLAGS) -o $@ $^ -lrt

clean:
	rm -f server client
