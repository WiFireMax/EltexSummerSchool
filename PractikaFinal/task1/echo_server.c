#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define SERVER_PORT 34343
#define BUF_SIZE 1024
#define MAX_CLIENTS 100

typedef struct
{
    struct sockaddr_in addr;
    int msg_count;
} client_info_t;

client_info_t clients[MAX_CLIENTS];
int client_count = 0;

int cmp_sockaddr(struct sockaddr_in *a, struct sockaddr_in *b)
{
    return (a->sin_addr.s_addr == b->sin_addr.s_addr) &&
           (a->sin_port == b->sin_port);
}

int find_or_add_client(struct sockaddr_in *addr)
{
    for (int i = 0; i < client_count; i++)
    {
        if (cmp_sockaddr(&(clients[i].addr), addr))
        {
            return i;
        }
    }
    if (client_count < MAX_CLIENTS) {
        clients[client_count].addr = *addr;
        clients[client_count].msg_count = 0;
        return client_count++;
    }
    return -1;
}

int main()
{
    int sockfd;
    char buffer[BUF_SIZE];
    struct sockaddr_in server_addr, client_addr;
    socklen_t addr_len = sizeof(client_addr);

    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
    {
        perror("socket creation failed");
        exit(EXIT_FAILURE);
    }

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(SERVER_PORT);

    if (bind(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
    {
        perror("bind failed");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    printf("UDP echo server started on port %d\n", SERVER_PORT);

    while (1)
    {
        ssize_t n = recvfrom(sockfd, buffer, BUF_SIZE - 1, 0, (struct sockaddr *)&client_addr, &addr_len);
        if (n < 0)
        {
            perror("recvfrom error");
            continue;
        }
        buffer[n] = '\0';

        int idx = find_or_add_client(&client_addr);
        if (idx < 0)
        {
            fprintf(stderr, "Too many clients\n");
            continue;
        }

        if (strcmp(buffer, "exit") == 0)
        {
            printf("Client %s:%d disconnected\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));
            clients[idx].msg_count = 0;
            continue;
        }

        clients[idx].msg_count++;

        char reply[BUF_SIZE];
        snprintf(reply, BUF_SIZE, "%s %d", buffer, clients[idx].msg_count);

        ssize_t sent = sendto(sockfd, reply, strlen(reply), 0, (struct sockaddr *)&client_addr, addr_len);
        if (sent < 0)
        {
            perror("sendto error");
        } else {
            printf("Client %s:%d msg #%d: %s\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port), clients[idx].msg_count, buffer);
        }
    }

    close(sockfd);
    return 0;
}
