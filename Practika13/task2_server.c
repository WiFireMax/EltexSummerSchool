#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>

#define SERVER_KEY 0x1234
#define MAX_CLIENTS 10

struct chat_msg
{
    long mtype; //1 - new user, 2 - chat
    pid_t pid;
    char name[32];
    char text[256];
};

struct client
{
    pid_t pid;
    char name[32];
    int queue_id;
};

struct client clients[MAX_CLIENTS];
int client_count = 0;

int server_queue_id;

void cleanup(int signum)
{
    printf("Cleaning up...\n");
    if (server_queue_id != -1)
    {
        msgctl(server_queue_id, IPC_RMID, NULL);
    }
    exit(0);
}

int find_client(pid_t pid)
{
    for (int i = 0; i < client_count; i++)
    {
        if (clients[i].pid == pid) return i;
    }
    return -1;
}

void broadcast(struct chat_msg *msg)
{
    for (int i = 0; i < client_count; i++)
    {
        if (msgsnd(clients[i].queue_id, msg, sizeof(struct chat_msg) - sizeof(long), 0) == -1)
        {
            perror("msgsnd broadcast");
        }
    }
}

int main()
{
    key_t key = SERVER_KEY;
    server_queue_id = msgget(key, IPC_CREAT | 0666);
    if (server_queue_id == -1)
    {
        perror("msgget server");
        exit(1);
    }

    signal(SIGINT, cleanup);

    printf("Server started, queue id: %d\n", server_queue_id);

    struct chat_msg msg;

    while (1) {
        ssize_t rcv = msgrcv(server_queue_id, &msg, sizeof(struct chat_msg) - sizeof(long), 0, 0);
        if (rcv == -1)
        {
            perror("msgrcv");
            continue;
        }

        if (msg.mtype == 1)
        {
            printf("New client connected: %s pid=%d\n", msg.name, msg.pid);

            if (client_count >= MAX_CLIENTS)
            {
                fprintf(stderr, "Max clients reached\n");
                continue;
            }

            if (find_client(msg.pid) != -1)
            {
                printf("Client with pid %d already exists\n", msg.pid);
                continue;
            }

            key_t client_key = SERVER_KEY + msg.pid;
            int client_queue = msgget(client_key, IPC_CREAT | 0666);
            if (client_queue == -1)
            {
                perror("msgget client queue");
                continue;
            }

            clients[client_count].pid = msg.pid;
            strncpy(clients[client_count].name, msg.name, 31);
            clients[client_count].queue_id = client_queue;
            client_count++;

            struct chat_msg notif;
            notif.mtype = 2;
            notif.pid = msg.pid;
            snprintf(notif.text, sizeof(notif.text), "User %s joined the chat", msg.name);
            strncpy(notif.name, "Server", 31);

            broadcast(&notif);

        } else if (msg.mtype == 2)
        {
            int idx = find_client(msg.pid);
            if (idx == -1)
            {
                fprintf(stderr, "Unknown client %d sent message\n", msg.pid);
                continue;
            }
            strncpy(msg.name, clients[idx].name, 31);
            printf("Message from %s: %s\n", msg.name, msg.text);

            broadcast(&msg);
        }
    }

    cleanup(0);

    return 0;
}
