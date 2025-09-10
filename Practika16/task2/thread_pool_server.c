#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <arpa/inet.h>
#include "time_ask.h"

#define PORT 34343
#define BACKLOG 10
#define THREAD_POOL_SIZE 4
#define TIME_STR_SIZE 64

typedef struct {
    int client_sock;
    int busy;
    pthread_t thread;
} worker_t;

worker_t workers[THREAD_POOL_SIZE];
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;

void* worker_function(void *arg)
{
    worker_t *worker = (worker_t*)arg;

    while (1)
    {
        pthread_mutex_lock(&mutex);
        while (!worker->busy)
            pthread_cond_wait(&cond, &mutex);
        pthread_mutex_unlock(&mutex);

        char buffer[TIME_STR_SIZE] = {0};
        ssize_t received = recv(worker->client_sock, buffer, sizeof(buffer)-1, 0);
        if (received <= 0)
        {
            if(received == 0)
                printf("Client closed connection\n");
            else
                perror("recv");
            close(worker->client_sock);

            pthread_mutex_lock(&mutex);
            worker->busy = 0;
            pthread_cond_signal(&cond);
            pthread_mutex_unlock(&mutex);
            continue;
        }
        buffer[received] = 0;
        printf("Worker received: '%s'\n", buffer);
        get_current_time_str(buffer, sizeof(buffer));
        ssize_t sent = send(worker->client_sock, buffer, strlen(buffer), 0);
        if(sent < 0)
            perror("send");

        close(worker->client_sock);

        pthread_mutex_lock(&mutex);
        worker->busy = 0;
        pthread_cond_signal(&cond);
        pthread_mutex_unlock(&mutex);
    }

    return NULL;
}

int main()
{
    int server_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (server_sock < 0) { perror("socket"); return 1; }

    struct sockaddr_in server_addr = {0};
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = INADDR_ANY;

    if (bind(server_sock, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0)
    {
        perror("bind");
        close(server_sock);
        return 1;
    }

    if (listen(server_sock, BACKLOG) < 0)
    {
        perror("listen");
        close(server_sock);
        return 1;
    }

    for (int i = 0; i < THREAD_POOL_SIZE; i++)
    {
        workers[i].busy = 0;
        if (pthread_create(&workers[i].thread, NULL, worker_function, &workers[i]) != 0)
        {
            perror("pthread_create");
            exit(1);
        }
    }

    printf("Server with thread pool listening on port %d\n", PORT);

    while (1)
    {
        struct sockaddr_in client_addr;
        socklen_t client_len = sizeof(client_addr);
        int client_sock = accept(server_sock, (struct sockaddr*)&client_addr, &client_len);
        if (client_sock < 0)
        {
            perror("accept");
            continue;
        }

        pthread_mutex_lock(&mutex);
        int assigned = 0;
        for (int i = 0; i < THREAD_POOL_SIZE; i++)
        {
            if (!workers[i].busy) {
                workers[i].client_sock = client_sock;
                workers[i].busy = 1;
                pthread_cond_signal(&cond);
                assigned = 1;
                break;
            }
        }
        pthread_mutex_unlock(&mutex);

        if (!assigned) {
            const char *msg = "Server too busy\n";
            send(client_sock, msg, strlen(msg), 0);
            close(client_sock);
            printf("Rejected client: server busy\n");
        }
    }

    close(server_sock);
    return 0;
}
