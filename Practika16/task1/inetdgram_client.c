#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>

#define PORT 34343
#define SERVER_IP "127.0.0.1"

int main()
{
    int sock;
    struct sockaddr_in server_addr;
    char buf[100];

    sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock < 0) {
        perror("socket");
        exit(1);
    }

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    inet_pton(AF_INET, SERVER_IP, &server_addr.sin_addr);

    const char *msg = "hello!";
    if (sendto(sock, msg, strlen(msg), 0, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
    {
        perror("sendto");
        close(sock);
        exit(1);
    }

    ssize_t n = recv(sock, buf, sizeof(buf) - 1, 0);
    if (n < 0) perror("recv");
    else {
        buf[n] = '\0';
        printf("Client received: %s\n", buf);
    }

    close(sock);
    return 0;
}
