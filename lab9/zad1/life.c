#include <ncurses.h>
#include <locale.h>
#include <unistd.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>
#include "grid.h"

pthread_t *threads; 

typedef struct {
    int start_row;
    int end_row;
    char *src;
    char *dst;
} ThreadArgs;

void *thread_work(void *args) {
    ThreadArgs *ta = (ThreadArgs *)args;
    for (int i = ta->start_row; i < ta->end_row; ++i) {
        for (int j = 0; j < grid_width; ++j) {
            ta->dst[i * grid_width + j] = is_alive(i, j, ta->src);
        }
    }
    return NULL;
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s num_threads\n", argv[0]);
        return EXIT_FAILURE;
    }

    int num_threads = atoi(argv[1]);

    if (num_threads > grid_height) {
        num_threads = grid_height;
    }

    threads = malloc(num_threads * sizeof(pthread_t));
    if (threads == NULL) {
        fprintf(stderr, "Error malloc\n");
        return EXIT_FAILURE;
    }

    srand(time(NULL));
    setlocale(LC_CTYPE, "");
    initscr();

    char *foreground = create_grid();
    char *background = create_grid();
    char *tmp;

    init_grid(foreground);

    while (true) {
        draw_grid(foreground);
        usleep(500 * 1000);

        int rows_per_thread = grid_height / num_threads;
        ThreadArgs args[num_threads];
        for (int i = 0; i < num_threads; ++i) {
            args[i].start_row = i * rows_per_thread;
            args[i].end_row = (i == num_threads - 1) ? grid_height : (i + 1) * rows_per_thread;
            args[i].src = foreground;
            args[i].dst = background;
            pthread_create(&threads[i], NULL, thread_work, &args[i]);
        }

        for (int i = 0; i < num_threads; ++i) {
            pthread_join(threads[i], NULL);
        }

        tmp = foreground;
        foreground = background;
        background = tmp;
    }

    endwin();
    destroy_grid(foreground);
    destroy_grid(background);
    free(threads);

    return EXIT_SUCCESS;
}
