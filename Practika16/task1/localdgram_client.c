#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <string.h>

#define CLIENT_SOCK_PATH "/tmp/unix_dgram_client_socket"
#define SOCK_PATH "/tmp/unix_dgram_socket"

int main()
{
    int sock;
    struct sockaddr_un server_addr, client_addr;
    char buf[100];

    sock = socket(AF_LOCAL, SOCK_DGRAM, 0);
    if (sock < 0) {
        perror("socket");
        exit(1);
    }

    unlink(CLIENT_SOCK_PATH);
    memset(&client_addr, 0, sizeof(client_addr));
    client_addr.sun_family = AF_LOCAL;
    strncpy(client_addr.sun_path, CLIENT_SOCK_PATH, sizeof(client_addr.sun_path) - 1);

    if (bind(sock, (struct sockaddr*)&client_addr, sizeof(client_addr)) < 0)
    {
        perror("bind");
        close(sock);
        exit(1);
    }
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sun_family = AF_LOCAL;
    strncpy(server_addr.sun_path, SOCK_PATH, sizeof(server_addr.sun_path) - 1);

    const char *msg = "hello!";
    if (sendto(sock, msg, strlen(msg), 0, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
    {
        perror("sendto");
        close(sock);
        unlink(CLIENT_SOCK_PATH);
        exit(1);
    }
    ssize_t n = recv(sock, buf, sizeof(buf) - 1, 0);
    if (n < 0) perror("recv");
    else {
        buf[n] = '\0';
        printf("Client received: %s\n", buf);
    }

    close(sock);
    unlink(CLIENT_SOCK_PATH);
    return 0;
}
