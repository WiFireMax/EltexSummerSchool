#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 5000
#define MCAST_GROUP "239.0.0.1"

int main() {
    int sockfd;
    struct sockaddr_in mcast_addr;
    const char *message = "Hello multicast!";

    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("socket creation failed");
        exit(EXIT_FAILURE);
    }

    memset(&mcast_addr, 0, sizeof(mcast_addr));
    mcast_addr.sin_family = AF_INET;
    mcast_addr.sin_port = htons(PORT);
    if (inet_pton(AF_INET, MCAST_GROUP, &mcast_addr.sin_addr) <= 0) {
        perror("inet_pton failed");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    if (sendto(sockfd, message, strlen(message), 0,
               (struct sockaddr *)&mcast_addr, sizeof(mcast_addr)) < 0) {
        perror("sendto failed");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    printf("Multicast message sent: %s\n", message);

    close(sockfd);
    return 0;
}
