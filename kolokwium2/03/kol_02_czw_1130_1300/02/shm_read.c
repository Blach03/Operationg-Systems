#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/shm.h>
#include <sys/ipc.h>
#include <string.h>
#include <errno.h>

#define PATH "/tmp"
#define BUFFER_SIZE 1024
#define ID 0

int main(int argc, char const *argv[])
{
	char * shmAddr;

    // przekształć ścieżkę i identyfikator na klucz komunikacji międzyprocesowej (IPC) Systemu V
	key_t key= ... (PATH,ID);

	// Utwórz segment pamięci współdzielonej
    // zwracaną wartością jest identyfikator bloku pamięci współdzielonej - czyli shmid
	int shmid;
	if ((shmid= ... ==-1)
	{
		fprintf(stderr, "... :%s\n", strerror(errno));
		exit(1);
	}

	// Mapuj segmenty pamięci współdzielonej na przestrzeń adresową procesu
	shmAddr = ... (shmid, ... );
	if(shmAddr==...)
	{
		fprintf(stderr, ":%s\n", strerror(errno));
	}	

	printf("%s\n",shmAddr);

	// Rozłącz 
	...
	// Usuń segment pamięci współdzielonej
	...

	return 0;
}

/*
gcc -o shm_read shm_read.c
*/
