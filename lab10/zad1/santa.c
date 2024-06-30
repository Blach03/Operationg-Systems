#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

#define NUM_REINDEER 9
#define DELIVERY_ROUNDS 4

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t santa_wake_cond = PTHREAD_COND_INITIALIZER;

int reindeer_back_count = 0;
int deliveries_done = 0;

void *reindeer(void *id) {
    int reindeer_id = *((int *)id);
    free(id);

    while (1) {
        int vacation_time = rand() % 6 + 5;
        sleep(vacation_time);

        pthread_mutex_lock(&mutex);
        reindeer_back_count++;
        printf("Renifer: czeka %d reniferów na Mikołaja, %d\n", reindeer_back_count, reindeer_id);

        if (reindeer_back_count == NUM_REINDEER) {
            printf("Renifer: wybudzam Mikołaja, %d\n", reindeer_id);
            pthread_cond_signal(&santa_wake_cond);
        }

        pthread_mutex_unlock(&mutex);

        int delivery_time = rand() % 3 + 2;
        sleep(delivery_time);
    }

    return NULL;
}

void *santa(void *arg) {
    (void)arg;

    while (1) {
        pthread_mutex_lock(&mutex);

        while (reindeer_back_count < NUM_REINDEER) {
            pthread_cond_wait(&santa_wake_cond, &mutex);
        }

        printf("Mikołaj: budzę się\n");
        printf("Mikołaj: dostarczam zabawki\n");

        reindeer_back_count = 0;
        deliveries_done++;

        int delivery_time = rand() % 3 + 2;
        sleep(delivery_time);

        printf("Mikołaj: zasypiam\n");

        if (deliveries_done >= DELIVERY_ROUNDS) {
            pthread_mutex_unlock(&mutex);
            break;
        }

        pthread_mutex_unlock(&mutex);
    }

    return NULL;
}

int main() {
    pthread_t santa_thread;
    pthread_t reindeer_threads[NUM_REINDEER];

    srand(time(NULL));

    pthread_create(&santa_thread, NULL, santa, NULL);

    for (int i = 0; i < NUM_REINDEER; i++) {
        int *id = malloc(sizeof(int));
        *id = i + 1;
        pthread_create(&reindeer_threads[i], NULL, reindeer, id);
    }

    pthread_join(santa_thread, NULL);

    for (int i = 0; i < NUM_REINDEER; i++) {
        pthread_cancel(reindeer_threads[i]);
        pthread_join(reindeer_threads[i], NULL);
    }

    pthread_mutex_destroy(&mutex);
    pthread_cond_destroy(&santa_wake_cond);

    return 0;
}
