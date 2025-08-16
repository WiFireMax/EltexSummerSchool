#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <string.h>

#define SOCK_PATH "/tmp/unix_stream_socket"

int main()
{
    int sock, client_sock;
    struct sockaddr_un server_addr;
    char buf[100];

    sock = socket(AF_LOCAL, SOCK_STREAM, 0);
    if (sock < 0) {
        perror("socket");
        exit(1);
    }

    unlink(SOCK_PATH);
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sun_family = AF_LOCAL;
    strncpy(server_addr.sun_path, SOCK_PATH, sizeof(server_addr.sun_path) - 1);

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
    unlink(SOCK_PATH);
    return 0;
}
