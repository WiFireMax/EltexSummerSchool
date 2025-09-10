#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/select.h>
#include "time_ask.h"

#define PORT 34343
#define TIME_STR_SIZE 64
#define BUF_SIZE 64

int main()
{
    int tcp_sock = socket(AF_INET, SOCK_STREAM, 0);
    int udp_sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (tcp_sock < 0 || udp_sock < 0)
    {
        perror("socket");
        return 1;
    }

    struct sockaddr_in addr = {0};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(PORT);
    addr.sin_addr.s_addr = INADDR_ANY;

    if (bind(tcp_sock, (struct sockaddr*)&addr, sizeof(addr)) < 0)
    {
        perror("bind tcp");
        return 1;
    }
    if (bind(udp_sock, (struct sockaddr*)&addr, sizeof(addr)) < 0)
    {
        perror("bind udp");
        return 1;
    }
    if (listen(tcp_sock, 10) < 0)
    {
        perror("listen");
        return 1;
    }

    printf("Server TCP+UDP with select listening on port %d\n", PORT);

    fd_set read_fds;
    int max_fd = (tcp_sock > udp_sock) ? tcp_sock : udp_sock;
    char buffer[BUF_SIZE];

    while (1)
    {
        FD_ZERO(&read_fds);
        FD_SET(tcp_sock, &read_fds);
        FD_SET(udp_sock, &read_fds);

        int activity = select(max_fd + 1, &read_fds, NULL, NULL, NULL);
        if (activity < 0)
        {
            perror("select");
            continue;
        }

        if (FD_ISSET(tcp_sock, &read_fds))
        {
            struct sockaddr_in client_addr;
            socklen_t client_len = sizeof(client_addr);
            int client_sock = accept(tcp_sock, (struct sockaddr*)&client_addr, &client_len);
            if (client_sock >= 0)
            {
                ssize_t received = recv(client_sock, buffer, BUF_SIZE-1, 0);
                if (received > 0)
                {
                    buffer[received] = 0;
                    printf("TCP client says: %s\n", buffer);
                    get_current_time_str(buffer, BUF_SIZE);
                    ssize_t sent = send(client_sock, buffer, strlen(buffer), 0);
                    if(sent < 0)
                        perror("send tcp");
                } else if(received == 0) {
                    printf("TCP client closed connection\n");
                } else {
                    perror("recv tcp");
                }
                close(client_sock);
            }
        }

        if (FD_ISSET(udp_sock, &read_fds))
        {
            struct sockaddr_in client_addr;
            socklen_t client_len = sizeof(client_addr);
            ssize_t len = recvfrom(udp_sock, buffer, BUF_SIZE-1, 0, (struct sockaddr*)&client_addr, &client_len);
            if (len > 0)
            {
                buffer[len] = 0;
                printf("UDP client says: %s\n", buffer);
                get_current_time_str(buffer, BUF_SIZE);
                ssize_t sent = sendto(udp_sock, buffer, strlen(buffer), 0, (struct sockaddr*)&client_addr, client_len);
                if(sent < 0)
                    perror("sendto udp");
            }
        }
    }

    close(tcp_sock);
    close(udp_sock);
    return 0;
}
