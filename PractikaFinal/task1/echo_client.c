#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define SERVER_PORT 34343
#define BUF_SIZE 1024

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        fprintf(stderr, "Usage: %s <server_ip>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    int sockfd;
    struct sockaddr_in server_addr;
    char buffer[BUF_SIZE];
    socklen_t addr_len = sizeof(server_addr);

    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
    {
        perror("socket creation failed");
        exit(EXIT_FAILURE);
    }

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(SERVER_PORT);

    if (inet_pton(AF_INET, argv[1], &server_addr.sin_addr) <= 0)
    {
        fprintf(stderr, "Invalid address: %s\n", argv[1]);
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    printf("Enter messages to send to server (type 'exit' to quit):\n");

    while (1)
    {
        printf("> ");
        if (!fgets(buffer, BUF_SIZE, stdin))
        {
            break;
        }
        size_t len = strlen(buffer);
        if (buffer[len - 1] == '\n')
            buffer[len - 1] = '\0';

        ssize_t sent = sendto(sockfd, buffer, strlen(buffer), 0, (struct sockaddr *)&server_addr, addr_len);
        if (sent < 0)
        {
            perror("sendto failed");
            continue;
        }

        if (strcmp(buffer, "exit") == 0)
        {
            printf("Exiting...\n");
            break;
        }

        ssize_t n = recvfrom(sockfd, buffer, BUF_SIZE - 1, 0, NULL, NULL);
        if (n < 0)
        {
            perror("recvfrom failed");
            continue;
        }
        buffer[n] = '\0';
        printf("Server replied: %s\n", buffer);
    }

    close(sockfd);
    return 0;
}
