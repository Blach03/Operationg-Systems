CC = gcc
CFLAGS = -g -Wall

LIB_NAME = library

all: static shared dynamic

static: library_static client_static

shared: library_shared client_shared

dynamic: library_shared client_dynamic

library_static: library.o
	ar rcs lib$(LIB_NAME).a library.o

library_shared: library.o
	$(CC) -shared -o lib$(LIB_NAME).so library.o

client_static: client.o
	$(CC) -o client_static client.o -L. -l$(LIB_NAME) -DSTATIC

client_shared: client.o
	$(CC) -o client_shared client.o -L. -l$(LIB_NAME) -DSHARED

client_dynamic: client.o
	$(CC) -o client_dynamic client.o -DDYNAMIC -ldl

library.o: library.c
	$(CC) $(CFLAGS) -c -fPIC library.c -o library.o

client.o: client.c
	$(CC) $(CFLAGS) -c client.c -o client.o

clean:
	rm -f *.o *.a *.so client_static client_shared client_dynamic
