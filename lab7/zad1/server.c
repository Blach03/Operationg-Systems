#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <unistd.h>

#define MAX_CLIENTS 10
#define MAX_TEXT 512
#define SERVER_QUEUE_KEY 12345
#define INIT 1
#define CLIENT_MSG 2

struct message {
    long type;
    int client_id;
    char text[MAX_TEXT];
};

int client_queues[MAX_CLIENTS] = {0};

void send_to_all_clients(struct message *msg) {
    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (client_queues[i] != 0 && i != msg->client_id) {
            msgsnd(client_queues[i], msg, sizeof(struct message) - sizeof(long), 0);
        }
    }
}

int main() {
    int server_queue_id = msgget(SERVER_QUEUE_KEY, IPC_CREAT | 0666);
    if (server_queue_id == -1) {
        perror("msgget");
        exit(1);
    }

    int next_client_id = 0;
    struct message msg;

    while (1) {
        if (msgrcv(server_queue_id, &msg, sizeof(struct message) - sizeof(long), 0, 0) == -1) {
            perror("msgrcv");
            exit(1);
        }

        if (msg.type == INIT) {
            int client_id = next_client_id++;
            client_queues[client_id] = msg.client_id;
            msg.client_id = client_id;
            msgsnd(client_queues[client_id], &msg, sizeof(struct message) - sizeof(long), 0);
        } else if (msg.type == CLIENT_MSG) {
            send_to_all_clients(&msg);
        }
    }
}
