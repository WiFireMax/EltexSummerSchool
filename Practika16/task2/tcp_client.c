#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include "time_ask.h"

#define SERVER_IP "127.0.0.1"
#define SERVER_PORT 34343
#define TIME_STR_SIZE 64

int main()
{
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0)
    {
        perror("socket");
        return 1;
    }

    struct sockaddr_in server_addr = {0};
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(SERVER_PORT);
    inet_pton(AF_INET, SERVER_IP, &server_addr.sin_addr);

    if (connect(sock, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0)
    {
        perror("connect");
        close(sock);
        return 1;
    }

    if (send(sock, "", 1, 0) < 0) {
        perror("send");
        close(sock);
        return 1;
    }


    char buffer[TIME_STR_SIZE] = {0};

    ssize_t len = recv(sock, buffer, TIME_STR_SIZE - 1, 0);
    if (len < 0)
    {
        perror("recv");
    } else {
        buffer[len] = 0;
        printf("Server time: %s\n", buffer);
    }

    close(sock);
    return 0;
}
