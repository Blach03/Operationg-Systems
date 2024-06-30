#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <unistd.h>

#define MAX_TEXT 512
#define SERVER_QUEUE_KEY 12345
#define INIT 1
#define CLIENT_MSG 2

struct message {
    long type;
    int client_id;
    char text[MAX_TEXT];
};

void listen_for_messages(int queue_id) {
    struct message msg;
    while (1) {
        if (msgrcv(queue_id, &msg, sizeof(struct message) - sizeof(long), 0, 0) == -1) {
            perror("msgrcv");
            exit(1);
        }
        printf("Message from %d: %s\n", msg.client_id, msg.text);
    }
}

int main() {
    int client_queue_id = msgget(IPC_PRIVATE, IPC_CREAT | 0666);
    if (client_queue_id == -1) {
        perror("msgget");
        exit(1);
    }

    struct message msg;
    msg.type = INIT;
    msg.client_id = client_queue_id;

    int server_queue_id = msgget(SERVER_QUEUE_KEY, 0);
    if (server_queue_id == -1) {
        perror("msgget");
        exit(1);
    }

    if (msgsnd(server_queue_id, &msg, sizeof(struct message) - sizeof(long), 0) == -1) {
        perror("msgsnd");
        exit(1);
    }

    if (msgrcv(client_queue_id, &msg, sizeof(struct message) - sizeof(long), 0, 0) == -1) {
        perror("msgrcv");
        exit(1);
    }

    int client_id = msg.client_id;
    printf("Client ID: %d\n", client_id);

    if (fork() == 0) {
        listen_for_messages(client_queue_id);
        exit(0);
    }

    while (1) {
        msg.type = CLIENT_MSG;
        msg.client_id = client_id;
        fgets(msg.text, MAX_TEXT, stdin);

        if (msgsnd(server_queue_id, &msg, sizeof(struct message) - sizeof(long), 0) == -1) {
            perror("msgsnd");
            exit(1);
        }
    }
}
