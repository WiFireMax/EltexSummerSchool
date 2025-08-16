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

    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        perror("socket");
        exit(1);
    }

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    inet_pton(AF_INET, SERVER_IP, &server_addr.sin_addr);

    if (connect(sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
    {
        perror("connect");
        close(sock);
        exit(1);
    }

    const char *msg = "hello!";
    write(sock, msg, strlen(msg));

    ssize_t n = read(sock, buf, sizeof(buf) - 1);
    if (n < 0) perror("read");
    else {
        buf[n] = '\0';
        printf("Client received: %s\n", buf);
    }

    close(sock);
    return 0;
}
