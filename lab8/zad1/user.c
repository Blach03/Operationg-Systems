#include <sys/shm.h>
#include <sys/sem.h>
#include <sys/ipc.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include "common.h"

#define QUEUE_KEY 1234
#define SEM_KEY 5678

void generate_random_string(char *str, size_t size) {
    const char charset[] = "abcdefghijklmnopqrstuvwxyz";
    for (size_t i = 0; i < size; i++) {
        str[i] = charset[rand() % (sizeof(charset) - 1)];
    }
    str[size] = '\0';
}

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

    srand(time(NULL) ^ getpid());

    while (1) {
        char job[JOB_SIZE + 1];
        generate_random_string(job, JOB_SIZE);

        sem_op(semid, 0, -1);

        if (queue->count < QUEUE_SIZE) {
            strncpy(queue->jobs[queue->rear], job, JOB_SIZE);
            queue->rear = (queue->rear + 1) % QUEUE_SIZE;
            queue->count++;
            printf("Added job: %s\n", job);
        } else {
            printf("Queue full, waiting...\n");
        }

        sem_op(semid, 0, 1);

        sleep(rand() % 5 + 1);
    }

    shmdt(queue);
    return 0;
}
