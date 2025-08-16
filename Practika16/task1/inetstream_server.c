#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <string.h>

#define PORT 34343

int main()
{
    int sock, client_sock;
    struct sockaddr_in server_addr;
    char buf[100];

    sock = socket(AF_INET, SOCK_STREAM, 0);
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

    if (listen(sock, 5) < 0)
    {
        perror("listen");
        close(sock);
        exit(1);
    }

    client_sock = accept(sock, NULL, NULL);
    if (client_sock < 0)
    {
        perror("accept");
        close(sock);
        exit(1);
    }

    ssize_t n = read(client_sock, buf, sizeof(buf) - 1);
    if (n < 0) perror("read");
    else {
        buf[n] = '\0';
        printf("Server got: %s\n", buf);
        write(client_sock, "hi!", 3);
    }

    close(client_sock);
    close(sock);
    return 0;
}
