#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>

int main() {
    DIR *dir;
    struct dirent *entry;
    struct stat buf;
    long long total_size = 0;

    dir = opendir(".");
    if (dir == NULL) {
        perror("Nie można otworzyć katalogu");
        exit(EXIT_FAILURE);
    }

    while ((entry = readdir(dir)) != NULL) {
        if (stat(entry->d_name, &buf) == -1) {
            perror("Błąd w odczycie informacji o pliku");
            exit(EXIT_FAILURE);
        }
        
        if (!S_ISDIR(buf.st_mode)) {
            printf("Nazwa: %s\tRozmiar: %lld\n", entry->d_name, (long long)buf.st_size);
            total_size += buf.st_size;
        }
    }

    closedir(dir);

    printf("\nSumaryczny rozmiar wszystkich plików: %lld\n", total_size);

    return 0;
}