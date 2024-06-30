#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <string.h>

void sigusr1_handler(int signum, siginfo_t *info, void *context) {
    printf("Otrzymano potwierdzenie od catcher, PID catcher: %d\n", info->si_pid);
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Sposób użycia: %s <PID_catcher> <tryb_pracy>\n", argv[0]);
        return 1;
    }

    pid_t catcher_pid = atoi(argv[1]);
    int tryb_pracy = atoi(argv[2]);

    struct sigaction sa;
    memset(&sa, 0, sizeof(sa));
    sa.sa_sigaction = sigusr1_handler;
    sa.sa_flags = SA_SIGINFO;

    if (sigaction(SIGUSR1, &sa, NULL) == -1) {
        perror("sigaction");
        return 1;
    }

    union sigval value;
    value.sival_int = tryb_pracy;

    if (sigqueue(catcher_pid, SIGUSR1, value) == -1) {
        perror("sigqueue");
        return 1;
    }

    pause();

    return 0;
}
