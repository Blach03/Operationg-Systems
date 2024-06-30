#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/wait.h>

#define FIFO_NAME "calka_fifo"

int main() {
    int fd;
    double poczatek, koniec;

    if (mkfifo(FIFO_NAME, 0666) == -1) {
        perror("Nie można utworzyć potoku nazwanego");
        exit(EXIT_FAILURE);
    }

    printf("Catcher czeka na dane...\n");

    fd = open(FIFO_NAME, O_RDONLY);
    if (fd == -1) {
        perror("Nie można otworzyć potoku do odczytu");
        exit(EXIT_FAILURE);
    }

    read(fd, &poczatek, sizeof(poczatek));
    read(fd, &koniec, sizeof(koniec));

    close(fd);

    double szerokosc_prostokata = 0.0001;
    double suma = 0.0;
    double x;
    for (x = poczatek; x < koniec; x += szerokosc_prostokata) {
        suma += (4.0 / (x * x + 1.0)) * szerokosc_prostokata;
    }

    fd = open(FIFO_NAME, O_WRONLY);
    if (fd == -1) {
        perror("Nie można otworzyć potoku do zapisu");
        exit(EXIT_FAILURE);
    }

    write(fd, &suma, sizeof(suma));
    close(fd);

    unlink(FIFO_NAME);

    return 0;
}
