#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <string.h>

volatile sig_atomic_t tryb_pracy = 0;

void sigusr1_handler(int signum, siginfo_t *info, void *context) {
    printf("Otrzymano sygnał SIGUSR1 od sender, PID sender: %d\n", info->si_pid);

    tryb_pracy = info->si_value.sival_int;
    
    if (kill(info->si_pid, SIGUSR1) == -1) {
        perror("kill");
        exit(1);
    }
}

void wykonaj_prace(int tryb) {
    switch (tryb) {
        case 1:
            for (int i = 1; i <= 100; ++i) {
                printf("%d\n", i);
                fflush(stdout);
            }
            break;
        case 2:
            printf("Liczba żądań zmiany trybu pracy: %d\n", tryb_pracy);
            fflush(stdout);
            break;
        case 3:
            printf("Zakończenie działania programu catcher\n");
            exit(0);
            break;
        default:
            printf("Nieznany tryb pracy: %d\n", tryb);
            fflush(stdout);
            break;
    }
}

int main() {
    printf("PID catcher: %d\n", getpid());

    struct sigaction sa;
    memset(&sa, 0, sizeof(sa));
    sa.sa_sigaction = sigusr1_handler;
    sa.sa_flags = SA_SIGINFO;

    if (sigaction(SIGUSR1, &sa, NULL) == -1) {
        perror("sigaction");
        return 1;
    }

    while (1) {
        pause();
        
        wykonaj_prace(tryb_pracy);
    }

    return 0;
}
