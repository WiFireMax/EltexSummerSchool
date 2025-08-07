// client_sysv.c
#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <string.h>

#define MSG_KEY 1234
#define MAX_TEXT 512

struct message
{
    long mtype;
    char mtext[MAX_TEXT];
};

int main()
{
    int msgid;
    struct message msg;

    msgid = msgget(MSG_KEY, 0666);
    if (msgid == -1)
    {
        perror("msgget");
        exit(1);
    }

    if (msgrcv(msgid, &msg, MAX_TEXT, 1, 0) == -1)
    {
        perror("msgrcv");
        exit(1);
    }
    printf("Client: Received message: %s\n", msg.mtext);

    msg.mtype = 2;
    strcpy(msg.mtext, "Hello!");
    if (msgsnd(msgid, &msg, strlen(msg.mtext)+1, 0) == -1)
    {
        perror("msgsnd");
        exit(1);
    }
    printf("Client: Sent message: Hello!\n");

    return 0;
}
