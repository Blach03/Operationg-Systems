#include <sys/shm.h>
#include <sys/sem.h>
#include <sys/ipc.h>
#include <stdlib.h>
#include <stdio.h>
#include "common.h"

#define QUEUE_KEY 1234
#define SEM_KEY 5678
#define PRINTERS 3

int main() {
    int shmid = shmget(QUEUE_KEY, sizeof(job_queue_t), IPC_CREAT | 0666);
    if (shmid == -1) {
        perror("shmget");
        exit(1);
    }

    job_queue_t *queue = (job_queue_t *)shmat(shmid, NULL, 0);
    queue->front = 0;
    queue->rear = 0;
    queue->count = 0;
    shmdt(queue);

    int semid = semget(SEM_KEY, 2, IPC_CREAT | 0666);
    if (semid == -1) {
        perror("semget");
        exit(1);
    }

    semctl(semid, 0, SETVAL, 1);
    semctl(semid, 1, SETVAL, PRINTERS);

    printf("Initialization completed.\n");
    return 0;
}
