#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

#define BLOCK_SIZE 1024

double current_timestamp() {
    struct timeval te; 
    gettimeofday(&te, NULL); 
    double milliseconds = (te.tv_sec  + te.tv_usec) / 1000.0;
    return milliseconds;
}

void reverse_file_one_char(const char *input_filename, const char *output_filename) {
    FILE *input_file = fopen(input_filename, "rb");
    FILE *output_file = fopen(output_filename, "wb");

    fseek(input_file, 0, SEEK_END);
    long long file_size = ftell(input_file);

    for (long long i = file_size - 1; i >= 0; i--) {
        fseek(input_file, i, SEEK_SET);
        char c;
        fread(&c, sizeof(char), 1, input_file);
        fwrite(&c, sizeof(char), 1, output_file);
    }

    fclose(input_file);
    fclose(output_file);
}

void reverse_file_blocks(const char *input_filename, const char *output_filename) {
    FILE *input_file = fopen(input_filename, "rb");
    FILE *output_file = fopen(output_filename, "wb");

    fseek(input_file, 0, SEEK_END);
    long long file_size = ftell(input_file);

    fseek(input_file, -(BLOCK_SIZE), SEEK_END);

    while (file_size > 0) {
        char buffer[BLOCK_SIZE];
        size_t bytes_to_read = (file_size >= BLOCK_SIZE) ? BLOCK_SIZE : file_size;

        fseek(input_file, -(long)(bytes_to_read), SEEK_CUR);
        fread(buffer, sizeof(char), bytes_to_read, input_file);

        for (size_t i = 0; i < bytes_to_read / 2; ++i) {
            char temp = buffer[i];
            buffer[i] = buffer[bytes_to_read - i - 1];
            buffer[bytes_to_read - i - 1] = temp;
        }

        fwrite(buffer, sizeof(char), bytes_to_read, output_file);

        file_size -= bytes_to_read;
        fseek(input_file, -(2 * BLOCK_SIZE), SEEK_CUR);
    }

    fclose(input_file);
    fclose(output_file);
}

int main(int argc, char *argv[]) {

    int mode = atoi(argv[3]);

    const char *input_filename = argv[1];
    const char *output_filename = argv[2];

    double start_time = current_timestamp();
    if (mode == 1)
        reverse_file_one_char(input_filename, output_filename);
    else if (mode == 2)
        reverse_file_blocks(input_filename, output_filename);
    double end_time = current_timestamp();

    printf("Execution time: %lld ms\n", end_time - start_time);

    return EXIT_SUCCESS;
}
