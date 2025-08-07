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

    server_mq = mq_open(SERVER_QUEUE_NAME, O_CREAT | O_RDONLY, 0644, &attr);
    if (server_mq == (mqd_t)-1)
    {
        perror("Server: mq_open(server)");
        exit(1);
    }

    client_mq = mq_open(CLIENT_QUEUE_NAME, O_WRONLY);
    if (client_mq == (mqd_t)-1)
    {
        perror("Server: mq_open(client)");
        mq_close(server_mq);
        mq_unlink(SERVER_QUEUE_NAME);
        exit(1);
    }

    if (mq_send(client_mq, "Hi!", strlen("Hi!") + 1, 0) == -1)
    {
        perror("Server: mq_send");
        mq_close(server_mq);
        mq_close(client_mq);
        mq_unlink(SERVER_QUEUE_NAME);
        mq_unlink(CLIENT_QUEUE_NAME);
        return 0;
    }

    printf("Server: Sent message: Hi!\n");

    ssize_t bytes_read = mq_receive(server_mq, buffer, MAX_SIZE, NULL);
    if (bytes_read >= 0) {
        buffer[bytes_read] = '\0';
        printf("Server: Received message: %s\n", buffer);
    } else {
        perror("Server: mq_receive");
    }
}
