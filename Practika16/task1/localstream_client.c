#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <string.h>

#define SOCK_PATH "/tmp/unix_stream_socket"

int main()
{
    int sock;
    struct sockaddr_un server_addr;
    char buf[100];

    sock = socket(AF_LOCAL, SOCK_STREAM, 0);
    if (sock < 0) {
        perror("socket");
        exit(1);
    }

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sun_family = AF_LOCAL;
    strncpy(server_addr.sun_path, SOCK_PATH, sizeof(server_addr.sun_path) - 1);

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
