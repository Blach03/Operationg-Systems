#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int global = 0;

int main(int argc, char *argv[]) {
    int local = 0;

    if (argc != 2) {
        fprintf(stderr, "Użycie: %s ścieżka katalogu\n", argv[0]);
        return EXIT_FAILURE;
    }

    pid_t pid = fork();

    if (pid == -1) {
        perror("Błąd funkcji fork");
        return EXIT_FAILURE;
    } else if (pid == 0) { // proces potomny
        printf("child process\n");
        global++;
        local++;
        printf("child pid = %d, parent pid = %d\n", getpid(), getppid());
        printf("child's local = %d, child's global = %d\n", local, global);

        execl("/bin/ls", "ls", argv[1], NULL);
        perror("Błąd funkcji execl");
        return EXIT_FAILURE;
    } else { // proces rodzica
        int status;
        wait(&status);
        printf("parent process\n");
        printf("parent pid = %d, child pid = %d\n", getpid(), pid);

        int child_exit_code;
        if (WIFEXITED(child_exit_code)) {
            printf("Child exit code: %d\n", WEXITSTATUS(child_exit_code));
        }

        printf("parent's local = %d, parent's global = %d\n", local, global);

        return EXIT_SUCCESS;
    }
}
