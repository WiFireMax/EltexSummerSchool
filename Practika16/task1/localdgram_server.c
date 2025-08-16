#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <string.h>

#define SOCK_PATH "/tmp/unix_dgram_socket"

int main()
{
    int sock;
    struct sockaddr_un server_addr, client_addr;
    char buf[100];
    socklen_t client_len = sizeof(client_addr);

    sock = socket(AF_LOCAL, SOCK_DGRAM, 0);
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

    ssize_t n = recvfrom(sock, buf, sizeof(buf) - 1, 0, (struct sockaddr *)&client_addr, &client_len);
    if (n < 0) perror("recvfrom");
    else {
        buf[n] = '\0';
        printf("Server got: %s\n", buf);

        const char *reply = "hi!";
        sendto(sock, reply, strlen(reply), 0, (struct sockaddr *)&client_addr, client_len);
    }

    close(sock);
    unlink(SOCK_PATH);
    return 0;
}
