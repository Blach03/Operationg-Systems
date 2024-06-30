#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/time.h>

double funkcja(double x) {
    return 4.0 / (x * x + 1.0);
}

double oblicz_calke(double szerokosc, double poczatek, double koniec) {
    double suma = 0.0;
    double x;
    for (x = poczatek; x < koniec; x += szerokosc) {
        suma += funkcja(x) * szerokosc;
    }
    return suma;
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        printf("Użycie: %s <ilosc_prostokatow> <liczba_procesow>\n", argv[0]);
        return 1;
    }

    double ilosc_prostokatow = atof(argv[1]);
    int liczba_procesow = atoi(argv[2]);

    double poczatek = 0.0;
    double koniec = 1.0;

    struct timeval start, end;
    double czas_wykonania;

    gettimeofday(&start, NULL);

    double przedzial = (koniec - poczatek) / liczba_procesow;
    int i;
    double wynik_czesciowy, suma_calki = 0.0;

    for (i = 0; i < liczba_procesow; i++) {
        double start = poczatek + i * przedzial;
        double end = start + przedzial;

        int fd[2];
        pipe(fd);

        pid_t pid = fork();

        if (pid == 0) {
            close(fd[0]);
            wynik_czesciowy = oblicz_calke(1/ilosc_prostokatow, start, end);
            write(fd[1], &wynik_czesciowy, sizeof(wynik_czesciowy));
            close(fd[1]);
            exit(0);
        } else if (pid > 0) {
            close(fd[1]);
            double wynik_czesciowy_potomnego;
            read(fd[0], &wynik_czesciowy_potomnego, sizeof(wynik_czesciowy_potomnego));
            suma_calki += wynik_czesciowy_potomnego;
            close(fd[0]);
            wait(NULL);
        } else {
            perror("Fork nie powiódł się.");
            return 1;
        }
    }
    gettimeofday(&end, NULL);
    czas_wykonania = (end.tv_sec - start.tv_sec) + ((end.tv_usec - start.tv_usec) / 1000000.0);

    printf("Wynik całkowania: %.10f\n", suma_calki);
    printf("Czas wykonania: %.6f sekund\n", czas_wykonania);

    return 0;
}
