CC = gcc
CFLAGS = -Wall -fPIC

all: bin/test

lib/libbibl1.so: bibl1.c
	mkdir -p lib
	$(CC) $(CFLAGS) -shared -o lib/libbibl1.so bibl1.c

bin/test: zad1.c lib/libbibl1.so
	mkdir -p bin
	$(CC) $(CFLAGS) zad1.c -o bin/test -ldl

test: bin/test
	export LD_LIBRARY_PATH=./lib:$$LD_LIBRARY_PATH && ./bin/test

clean:
	rm -f bin/test lib/libbibl1.so
	rmdir bin lib

.PHONY: all test clean
