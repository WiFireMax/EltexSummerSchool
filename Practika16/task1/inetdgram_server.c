#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <string.h>

#define PORT 34343

int main()
{
    int sock;
    struct sockaddr_in server_addr, client_addr;
    char buf[100];
    socklen_t client_len = sizeof(client_addr);

    sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock < 0) {
        perror("socket");
        exit(1);
    }

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    if (bind(sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
    {
        perror("bind");
        close(sock);
        exit(1);
    }

    ssize_t n = recvfrom(sock, buf, sizeof(buf) - 1, 0, (struct sockaddr *)&client_addr, &client_len);
    if (n < 0) perror("recvfrom");
    else {
        buf[n] = '\0';
        printf("Server got: %s\n", buf);

        const char *reply = "hi!";
        sendto(sock, reply, strlen(reply), 0, (struct sockaddr *)&client_addr, client_len);
    }

    close(sock);
    return 0;
}
