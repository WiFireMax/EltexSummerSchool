#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>

#define PORT 5000
#define MCAST_GROUP "239.0.0.1"
#define MAXLINE 1024

int main()
{
    int sockfd;
    struct sockaddr_in local_addr;
    struct ip_mreq mreq;
    char buffer[MAXLINE];

    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
    {
        perror("socket creation failed");
        exit(EXIT_FAILURE);
    }

    memset(&local_addr, 0, sizeof(local_addr));
    local_addr.sin_family = AF_INET;
    local_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    local_addr.sin_port = htons(PORT);

    if (bind(sockfd, (struct sockaddr *)&local_addr, sizeof(local_addr)) < 0)
    {
        perror("bind failed");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    mreq.imr_multiaddr.s_addr = inet_addr(MCAST_GROUP);
    mreq.imr_interface.s_addr = htonl(INADDR_ANY);
    if (setsockopt(sockfd, IPPROTO_IP, IP_ADD_MEMBERSHIP, &mreq, sizeof(mreq)) < 0)
    {
        perror("setsockopt IP_ADD_MEMBERSHIP failed");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    printf("Multicast server listening on group %s port %d\n", MCAST_GROUP, PORT);

    while (1)
    {
        ssize_t n = recv(sockfd, buffer, MAXLINE, 0);
        if (n < 0)
        {
            perror("recv failed");
            continue;
        }
        buffer[n] = '\0';
        printf("Received multicast message: %s\n", buffer);
    }

    close(sockfd);
    return 0;
}
