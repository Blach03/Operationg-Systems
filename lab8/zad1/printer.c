#include <sys/shm.h>
#include <sys/sem.h>
#include <sys/ipc.h>
#include <stdio.h>
#include <unistd.h>
#include "common.h"

#define QUEUE_KEY 1234
#define SEM_KEY 5678

void sem_op(int semid, int semnum, int op) {
    struct sembuf opbuf = { semnum, op, 0 };
    semop(semid, &opbuf, 1);
}

int main() {
    int shmid = shmget(QUEUE_KEY, sizeof(job_queue_t), 0666);
    if (shmid == -1) {
        perror("shmget");
        exit(1);
    }

    job_queue_t *queue = (job_queue_t *)shmat(shmid, NULL, 0);
    int semid = semget(SEM_KEY, 2, 0666);
    if (semid == -1) {
        perror("semget");
        exit(1);
    }

    while (1) {
        sem_op(semid, 1, -1);

        sem_op(semid, 0, -1);

        if (queue->count > 0) {
            char job[JOB_SIZE + 1];
            strncpy(job, queue->jobs[queue->front], JOB_SIZE);
            queue->front = (queue->front + 1) % QUEUE_SIZE;
            queue->count--;

            sem_op(semid, 0, 1);

            printf("Printing job: %s\n", job);
            for (int i = 0; i < JOB_SIZE; i++) {
                printf("%c", job[i]);
                fflush(stdout);
                sleep(1);
            }
            printf("\n");

            sem_op(semid, 1, 1);
        } else {
            sem_op(semid, 0, 1);
        }
    }

    shmdt(queue);
    return 0;
}
