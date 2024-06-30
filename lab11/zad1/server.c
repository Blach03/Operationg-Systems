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
    int socket;
    char id[50];
} Client;

Client clients[MAX_CLIENTS];
int client_count = 0;
pthread_mutex_t clients_mutex = PTHREAD_MUTEX_INITIALIZER;

void send_message_to_all(char *message, char *sender_id);
void send_message_to_one(char *message, char *sender_id, char *receiver_id);
void list_clients(int client_socket);
void handle_client(int client_socket);
void *ping_clients(void *arg);

           

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <port>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    printf("Server IP Address: 127.0.0.1 \n");

    int server_socket, client_socket;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_addr_len = sizeof(client_addr);
    pthread_t tid, ping_tid;

    if ((server_socket = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(atoi(argv[1]));

    if (bind(server_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1) {
        perror("bind");
        close(server_socket);
        exit(EXIT_FAILURE);
    }

    if (listen(server_socket, MAX_CLIENTS) == -1) {
        perror("listen");
        close(server_socket);
        exit(EXIT_FAILURE);
    }

    pthread_create(&ping_tid, NULL, ping_clients, NULL);

    printf("Server started on port %s\n", argv[1]);

    while (1) {
        if ((client_socket = accept(server_socket, (struct sockaddr*)&client_addr, &client_addr_len)) == -1) {
            perror("accept");
            continue;
        }

        pthread_create(&tid, NULL, (void *)handle_client, (void *)(intptr_t)client_socket);
    }

    close(server_socket);
    return 0;
}


void handle_client(int client_socket) {
    char buffer[BUFFER_SIZE];
    char client_id[50];
    int nbytes;

    if ((nbytes = recv(client_socket, client_id, sizeof(client_id), 0)) <= 0) {
        close(client_socket);
        return;
    }
    client_id[nbytes] = '\0';

    pthread_mutex_lock(&clients_mutex);
    if (client_count < MAX_CLIENTS) {
        clients[client_count].socket = client_socket;
        strcpy(clients[client_count].id, client_id);
        client_count++;
    } else {
        close(client_socket);
        pthread_mutex_unlock(&clients_mutex);
        return;
    }
    pthread_mutex_unlock(&clients_mutex);

    while ((nbytes = recv(client_socket, buffer, BUFFER_SIZE, 0)) > 0) {
        buffer[nbytes] = '\0';

        if (strncmp(buffer, "LIST", 4) == 0) {
            list_clients(client_socket);
        } else if (strncmp(buffer, "2ALL", 4) == 0) {
            send_message_to_all(buffer + 5, client_id);
        } else if (strncmp(buffer, "2ONE", 4) == 0) {
            char *receiver_id = strtok(buffer + 5, " ");
            char *message = strtok(NULL, "\n");
            send_message_to_one(message, client_id, receiver_id);
        } else if (strncmp(buffer, "STOP", 4) == 0) {
            break;
        }
    }

    pthread_mutex_lock(&clients_mutex);
    for (int i = 0; i < client_count; i++) {
        if (clients[i].socket == client_socket) {
            for (int j = i; j < client_count - 1; j++) {
                clients[j] = clients[j + 1];
            }
            client_count--;
            break;
        }
    }
    pthread_mutex_unlock(&clients_mutex);

    close(client_socket);
}

void send_message_to_all(char *message, char *sender_id) {
    char buffer[BUFFER_SIZE];
    time_t now = time(NULL);
    snprintf(buffer, BUFFER_SIZE, "%s [%s]: %s", sender_id, ctime(&now), message);

    pthread_mutex_lock(&clients_mutex);
    for (int i = 0; i < client_count; i++) {
        if (strcmp(clients[i].id, sender_id) != 0) {
            if (send(clients[i].socket, buffer, strlen(buffer), 0) == -1) {
                perror("send");
            }
        }
    }
    pthread_mutex_unlock(&clients_mutex);
}

void send_message_to_one(char *message, char *sender_id, char *receiver_id) {
    char buffer[BUFFER_SIZE];
    time_t now = time(NULL);
    snprintf(buffer, BUFFER_SIZE, "%s [%s]: %s", sender_id, ctime(&now), message);

    pthread_mutex_lock(&clients_mutex);
    for (int i = 0; i < client_count; i++) {
        if (strcmp(clients[i].id, receiver_id) == 0) {
            if (send(clients[i].socket, buffer, strlen(buffer), 0) == -1) {
                perror("send");
            }
            break;
        }
    }
    pthread_mutex_unlock(&clients_mutex);
}

void list_clients(int client_socket) {
    char buffer[BUFFER_SIZE];
    buffer[0] = '\0';

    pthread_mutex_lock(&clients_mutex);
    for (int i = 0; i < client_count; i++) {
        strcat(buffer, clients[i].id);
        strcat(buffer, "\n");
    }
    pthread_mutex_unlock(&clients_mutex);

    if (send(client_socket, buffer, strlen(buffer), 0) == -1) {
        perror("send");
    }
}

void *ping_clients(void *arg) {
    while (1) {
        sleep(30);

        pthread_mutex_lock(&clients_mutex);
        for (int i = 0; i < client_count; i++) {
            if (send(clients[i].socket, "ALIVE", 5, 0) == -1) {
                perror("send");
            }
        }
        pthread_mutex_unlock(&clients_mutex);
    }
}
