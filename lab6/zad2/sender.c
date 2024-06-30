#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#define FIFO_NAME "calka_fifo"

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Użycie: %s <początek_przedziału> <koniec_przedziału>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    double poczatek = atof(argv[1]);
    double koniec = atof(argv[2]);

    int fd;

    fd = open(FIFO_NAME, O_WRONLY);
    if (fd == -1) {
        perror("Nie można otworzyć potoku do zapisu");
        exit(EXIT_FAILURE);
    }

    write(fd, &poczatek, sizeof(poczatek));
    write(fd, &koniec, sizeof(koniec));
    close(fd);

    fd = open(FIFO_NAME, O_RDONLY);
    if (fd == -1) {
        perror("Nie można otworzyć potoku do odczytu");
        exit(EXIT_FAILURE);
    }

    double wynik;
    read(fd, &wynik, sizeof(wynik));
    printf("Wynik całkowania: %.10f\n", wynik);

    close(fd);

    return 0;
}
