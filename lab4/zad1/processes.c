#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Użycie: %s liczba procesów\n", argv[0]);
        return EXIT_FAILURE;
    }

    int num_processes = atoi(argv[1]);

    for (int i = 0; i < num_processes; i++) {
        pid_t pid = fork();

        if (pid == -1) {
            perror("Błąd funkcji fork");
            return EXIT_FAILURE;
        } else if (pid == 0) {
            printf("PID rodzica: %d, PID potomka: %d\n", getppid(), getpid());
            return EXIT_SUCCESS;
        }
    }

    for (int i = 0; i < num_processes; i++) {
        int status;
        wait(&status);
    }

    printf("%s\n", argv[1]);

    return EXIT_SUCCESS;
}
