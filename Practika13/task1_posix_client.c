// client_posix.c
#include <stdio.h>
#include <stdlib.h>
#include <mqueue.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>

#define SERVER_QUEUE_NAME   "/server_queue"
#define CLIENT_QUEUE_NAME   "/client_queue"
#define MAX_SIZE           1024

int main()
{
    mqd_t server_mq, client_mq;
    struct mq_attr attr;
    char buffer[MAX_SIZE + 1];

    attr.mq_flags = 0;
    attr.mq_maxmsg = 10;
    attr.mq_msgsize = MAX_SIZE;
    attr.mq_curmsgs = 0;

    client_mq = mq_open(CLIENT_QUEUE_NAME, O_CREAT | O_RDONLY, 0644, &attr);
    if (client_mq == (mqd_t)-1)
    {
        perror("Client: mq_open(client)");
        exit(1);
    }

    server_mq = mq_open(SERVER_QUEUE_NAME, O_WRONLY);
    if (server_mq == (mqd_t)-1)
    {
        perror("Client: mq_open(server)");
        mq_close(client_mq);
        mq_unlink(CLIENT_QUEUE_NAME);
        exit(1);
    }

    ssize_t bytes_read = mq_receive(client_mq, buffer, MAX_SIZE, NULL);
    if (bytes_read >= 0)
    {
        buffer[bytes_read] = '\0';
        printf("Client: Received message: %s\n", buffer);
    } else {
        perror("Client: mq_receive");
        mq_close(server_mq);
        mq_close(client_mq);
        mq_unlink(CLIENT_QUEUE_NAME);
        return 0;
    }

    if (mq_send(server_mq, "Hello!", strlen("Hello!") + 1, 0) == -1)
    {
        perror("Client: mq_send");
    } else {
        printf("Client: Sent message: Hello!\n");
    }
}
