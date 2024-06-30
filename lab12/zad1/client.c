#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <signal.h>

#define BUFFER_SIZE 1024

int sock;
struct sockaddr_in server_addr;
socklen_t addr_len = sizeof(server_addr);
char client_id[50];

void *receive_messages(void *arg);
void handle_exit(int sig);

int main(int argc, char *argv[]) {
    if (argc != 4) {
        fprintf(stderr, "Usage: %s <client_id> <server_ip> <server_port>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    strcpy(client_id, argv[1]);
    char *server_ip = argv[2];
    int server_port = atoi(argv[3]);

    if ((sock = socket(AF_INET, SOCK_DGRAM, 0)) == -1) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(server_port);
    if (inet_pton(AF_INET, server_ip, &server_addr.sin_addr) <= 0) {
        perror("inet_pton");
        close(sock);
        exit(EXIT_FAILURE);
    }

    char id_msg[BUFFER_SIZE];
    snprintf(id_msg, BUFFER_SIZE, "ID %s", client_id);
    if (sendto(sock, id_msg, strlen(id_msg), 0, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1) {
        perror("sendto");
        close(sock);
        exit(EXIT_FAILURE);
    }

    signal(SIGINT, handle_exit);
    pthread_t tid;
    if (pthread_create(&tid, NULL, receive_messages, NULL) != 0) {
        perror("pthread_create");
        close(sock);
        exit(EXIT_FAILURE);
    }

    char buffer[BUFFER_SIZE];
    while (fgets(buffer, BUFFER_SIZE, stdin) != NULL) {
        if (sendto(sock, buffer, strlen(buffer), 0, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1) {
            perror("sendto");
            break;
        }
    }

    close(sock);
    return 0;
}

void *receive_messages(void *arg) {
    char buffer[BUFFER_SIZE];
    int nbytes;

    while ((nbytes = recvfrom(sock, buffer, BUFFER_SIZE, 0, (struct sockaddr*)&server_addr, &addr_len)) > 0) {
        buffer[nbytes] = '\0';
        printf("%s\n", buffer);
    }

    return NULL;
}

void handle_exit(int sig) {
    sendto(sock, "STOP", 4, 0, (struct sockaddr*)&server_addr, sizeof(server_addr));
    close(sock);
    exit(0);
}
