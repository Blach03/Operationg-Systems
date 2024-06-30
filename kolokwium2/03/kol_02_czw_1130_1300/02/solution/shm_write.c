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
	void *shmAddr=NULL;
	char dataAddr[]="Hello";
	
    // Utwórz unikalny klucz do identyfikacji bieżącej komunikacji między procesami (IPC) Systemu V	
	key_t key=ftok(PATH,ID);
	
	// Utwórz segment pamięci współdzielonej
    // zwracaną wartością jest identyfikator bloku pamięci współdzielonej - czyli shmid
	int shmid;
	if ((shmid=shmget(key,BUFFER_SIZE,0666|IPC_CREAT))==-1)
	{
		fprintf(stderr, "shmget:%s\n", strerror(errno));
		exit(1);
	}

	//Mapuj segmenty pamięci współdzielonej na przestrzeń adresową procesu
	//extern void *shmat (int __shmid, const void *__shmaddr, int __shmflg)
	shmAddr=shmat(shmid,(void*)0,0);
	if(shmAddr==(void*)-1)
	{
		fprintf(stderr, "shmat:%s\n", strerror(errno));
	}

	// Skopiuj dataAddr do shmAddr
	strcpy(shmAddr,dataAddr);

	//disconnect the process memory with the shared memory
	if (shmdt(shmAddr)==-1)
	{
		fprintf(stderr, "shmdt:%s\n", strerror(errno));
	}
	return 0;
}

/*
gcc -o shm_write shm_write.c
*/
