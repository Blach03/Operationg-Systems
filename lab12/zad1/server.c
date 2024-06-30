#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <netdb.h>
#include <ifaddrs.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define MAX_CLIENTS 10
#define BUFFER_SIZE 1024

typedef struct {
    struct sockaddr_in addr;
    char id[50];
} Client;

Client clients[MAX_CLIENTS];
int client_count = 0;
pthread_mutex_t clients_mutex = PTHREAD_MUTEX_INITIALIZER;

void send_message_to_all(int sock, char *message, char *sender_id, struct sockaddr_in sender_addr);
void send_message_to_one(int sock, char *message, char *sender_id, char *receiver_id);
void list_clients(int sock, struct sockaddr_in client_addr);
void handle_client(int sock);
void *ping_clients(void *arg);

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <port>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    printf("Server IP Address: 127.0.0.1\n");

    int sock;
    struct sockaddr_in server_addr;

    if ((sock = socket(AF_INET, SOCK_DGRAM, 0)) == -1) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(atoi(argv[1]));

    if (bind(sock, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1) {
        perror("bind");
        close(sock);
        exit(EXIT_FAILURE);
    }

    pthread_t ping_tid;
    pthread_create(&ping_tid, NULL, ping_clients, (void *)&sock);

    printf("Server started on port %s\n", argv[1]);

    while (1) {
        handle_client(sock);
    }

    close(sock);
    return 0;
}

void handle_client(int sock) {
    char buffer[BUFFER_SIZE];
    struct sockaddr_in client_addr;
    socklen_t addr_len = sizeof(client_addr);
    int nbytes;

    while ((nbytes = recvfrom(sock, buffer, BUFFER_SIZE, 0, (struct sockaddr*)&client_addr, &addr_len)) > 0) {
        buffer[nbytes] = '\0';
        
        if (strncmp(buffer, "ID", 2) == 0) {
            char client_id[50];
            sscanf(buffer + 3, "%s", client_id);
            
            pthread_mutex_lock(&clients_mutex);
            if (client_count < MAX_CLIENTS) {
                clients[client_count].addr = client_addr;
                strcpy(clients[client_count].id, client_id);
                client_count++;
            }
            pthread_mutex_unlock(&clients_mutex);
        } else if (strncmp(buffer, "LIST", 4) == 0) {
            list_clients(sock, client_addr);
        } else if (strncmp(buffer, "2ALL", 4) == 0) {
            send_message_to_all(sock, buffer + 5, "", client_addr);
        } else if (strncmp(buffer, "2ONE", 4) == 0) {
            char *receiver_id = strtok(buffer + 5, " ");
            char *message = strtok(NULL, "\n");
            send_message_to_one(sock, message, "", receiver_id);
        } else if (strncmp(buffer, "STOP", 4) == 0) {
            pthread_mutex_lock(&clients_mutex);
            for (int i = 0; i < client_count; i++) {
                if (memcmp(&clients[i].addr, &client_addr, sizeof(client_addr)) == 0) {
                    for (int j = i; j < client_count - 1; j++) {
                        clients[j] = clients[j + 1];
                    }
                    client_count--;
                    break;
                }
            }
            pthread_mutex_unlock(&clients_mutex);
        }
    }
}

void send_message_to_all(int sock, char *message, char *sender_id, struct sockaddr_in sender_addr) {
    char buffer[BUFFER_SIZE];
    time_t now = time(NULL);
    snprintf(buffer, BUFFER_SIZE, "%s [%s]: %s", sender_id, ctime(&now), message);

    pthread_mutex_lock(&clients_mutex);
    for (int i = 0; i < client_count; i++) {
        if (memcmp(&clients[i].addr, &sender_addr, sizeof(sender_addr)) != 0) {
            if (sendto(sock, buffer, strlen(buffer), 0, (struct sockaddr*)&clients[i].addr, sizeof(clients[i].addr)) == -1) {
                perror("sendto");
            }
        }
    }
    pthread_mutex_unlock(&clients_mutex);
}

void send_message_to_one(int sock, char *message, char *sender_id, char *receiver_id) {
    char buffer[BUFFER_SIZE];
    time_t now = time(NULL);
    snprintf(buffer, BUFFER_SIZE, "%s [%s]: %s", sender_id, ctime(&now), message);

    pthread_mutex_lock(&clients_mutex);
    for (int i = 0; i < client_count; i++) {
        if (strcmp(clients[i].id, receiver_id) == 0) {
            if (sendto(sock, buffer, strlen(buffer), 0, (struct sockaddr*)&clients[i].addr, sizeof(clients[i].addr)) == -1) {
                perror("sendto");
            }
            break;
        }
    }
    pthread_mutex_unlock(&clients_mutex);
}

void list_clients(int sock, struct sockaddr_in client_addr) {
    char buffer[BUFFER_SIZE];
    buffer[0] = '\0';

    pthread_mutex_lock(&clients_mutex);
    for (int i = 0; i < client_count; i++) {
        strcat(buffer, clients[i].id);
        strcat(buffer, "\n");
    }
    pthread_mutex_unlock(&clients_mutex);

    if (sendto(sock, buffer, strlen(buffer), 0, (struct sockaddr*)&client_addr, sizeof(client_addr)) == -1) {
        perror("sendto");
    }
}

void *ping_clients(void *arg) {
    int sock = *(int *)arg;
    while (1) {
        sleep(30);

        pthread_mutex_lock(&clients_mutex);
        for (int i = 0; i < client_count; i++) {
            if (sendto(sock, "ALIVE", 5, 0, (struct sockaddr*)&clients[i].addr, sizeof(clients[i].addr)) == -1) {
                perror("sendto");
            }
        }
        pthread_mutex_unlock(&clients_mutex);
    }
    return NULL;
}
