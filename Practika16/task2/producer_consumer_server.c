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
#define QUEUE_SIZE 16

typedef struct
{
    int client_sock;
} task_t;

typedef struct
{
    task_t tasks[QUEUE_SIZE];
    int front;
    int rear;
    int count;
    pthread_mutex_t mutex;
    pthread_cond_t cond_nonempty;
    pthread_cond_t cond_nonfull;
} task_queue_t;

task_queue_t queue;

void task_queue_init(task_queue_t *q)
{
    q->front = 0;
    q->rear = 0;
    q->count = 0;
    pthread_mutex_init(&q->mutex, NULL);
    pthread_cond_init(&q->cond_nonempty, NULL);
    pthread_cond_init(&q->cond_nonfull, NULL);
}

void task_queue_push(task_queue_t *q, task_t task)
{
    pthread_mutex_lock(&q->mutex);
    while (q->count == QUEUE_SIZE)
        pthread_cond_wait(&q->cond_nonfull, &q->mutex);

    q->tasks[q->rear] = task;
    q->rear = (q->rear + 1) % QUEUE_SIZE;
    q->count++;
    pthread_cond_signal(&q->cond_nonempty);
    pthread_mutex_unlock(&q->mutex);
}

task_t task_queue_pop(task_queue_t *q)
{
    pthread_mutex_lock(&q->mutex);
    while (q->count == 0)
        pthread_cond_wait(&q->cond_nonempty, &q->mutex);

    task_t task = q->tasks[q->front];
    q->front = (q->front + 1) % QUEUE_SIZE;
    q->count--;
    pthread_cond_signal(&q->cond_nonfull);
    pthread_mutex_unlock(&q->mutex);
    return task;
}

void* worker_function(void *arg)
{
    (void)arg;

    while (1)
    {
        task_t task = task_queue_pop(&queue);

        char buffer[TIME_STR_SIZE] = {0};

        ssize_t received = recv(task.client_sock, buffer, sizeof(buffer)-1, 0);
        if (received <= 0)
        {
            if(received == 0)
                printf("Client closed connection\n");
            else
                perror("recv");
            close(task.client_sock);
            continue;
        }
        buffer[received] = 0;
        printf("Worker received: '%s'\n", buffer);

        get_current_time_str(buffer, sizeof(buffer));
        ssize_t sent = send(task.client_sock, buffer, strlen(buffer), 0);
        if(sent < 0)
            perror("send");

        close(task.client_sock);
    }

    return NULL;
}

int main()
{
    int server_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (server_sock < 0)
    {
        perror("socket");
        return 1;
    }

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

    task_queue_init(&queue);

    pthread_t threads[THREAD_POOL_SIZE];
    for (int i = 0; i < THREAD_POOL_SIZE; i++)
    {
        if (pthread_create(&threads[i], NULL, worker_function, NULL) != 0)
        {
            perror("pthread_create");
            exit(1);
        }
    }

    printf("Server with producer-consumer queue listening on port %d\n", PORT);

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

        task_t task = {.client_sock = client_sock};
        task_queue_push(&queue, task);
    }

    close(server_sock);
    return 0;
}
