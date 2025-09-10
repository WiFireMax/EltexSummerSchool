#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <arpa/inet.h>
#include "time_ask.h"

#define PORT 34343
#define BACKLOG 10
#define TIME_STR_SIZE 64

void* client_handler(void *arg)
{
    int client_sock = *(int*)arg;
    free(arg);

    char buffer[TIME_STR_SIZE] = {0};

    ssize_t received = recv(client_sock, buffer, sizeof(buffer)-1, 0);
    if (received < 0)
    {
        perror("recv");
        close(client_sock);
        pthread_exit(NULL);
    }
    if (received == 0)
    {
        printf("Client closed connection immediately\n");
        close(client_sock);
        pthread_exit(NULL);
    }

    buffer[received] = 0;

    printf("Received from client: '%s'\n", buffer);

    get_current_time_str(buffer, sizeof(buffer));
    send(client_sock, buffer, strlen(buffer), 0);

    close(client_sock);
    pthread_exit(NULL);
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

    printf("Server thread per client listening on port %d\n", PORT);

    while (1)
    {
        struct sockaddr_in client_addr;
        socklen_t client_len = sizeof(client_addr);
        int *client_sock = malloc(sizeof(int));
        *client_sock = accept(server_sock, (struct sockaddr*)&client_addr, &client_len);
        if (*client_sock < 0)
        {
            perror("accept");
            free(client_sock);
            continue;
        }

        pthread_t tid;
        pthread_create(&tid, NULL, client_handler, client_sock);
        pthread_detach(tid);
    }

    close(server_sock);
    return 0;
}
