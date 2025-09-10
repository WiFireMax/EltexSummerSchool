#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/ip.h>
#include <netinet/udp.h>
#include <arpa/inet.h>

struct udp_packet
{
    struct udphdr udp;
    char data[1024];
};

unsigned short csum(unsigned short *ptr,int nbytes)
{
    long sum;
    unsigned short oddbyte;
    short answer;

    sum=0;
    while (nbytes>1)
    {
        sum+=*ptr++;
        nbytes-=2;
    }
    if (nbytes==1)
    {
        oddbyte=0;
        *((u_char*)&oddbyte)=*(u_char*)ptr;
        sum+=oddbyte;
    }

    sum = (sum>>16)+(sum & 0xffff);
    sum = sum + (sum>>16);
    answer=(short)~sum;

    return(answer);
}

int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        fprintf(stderr, "Usage: %s <server_ip> <message>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    int sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_UDP);
    if (sockfd < 0)
    {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    struct sockaddr_in servaddr;
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(34343);
    servaddr.sin_addr.s_addr = inet_addr(argv[1]);

    struct udp_packet packet;
    memset(&packet, 0, sizeof(packet));
    strncpy(packet.data, argv[2], sizeof(packet.data) - 1);

    packet.udp.source = htons(54321);
    packet.udp.dest = htons(34343);
    packet.udp.len = htons(sizeof(struct udphdr) + strlen(packet.data));
    packet.udp.check = 0;

    ssize_t sent = sendto(sockfd, &packet, sizeof(struct udphdr) + strlen(packet.data), 0, (struct sockaddr*)&servaddr, sizeof(servaddr));
    if (sent < 0)
    {
        perror("sendto");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    printf("Packet sent with UDP header\n");
    close(sockfd);
    return 0;
}
