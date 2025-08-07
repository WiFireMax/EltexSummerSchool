#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <string.h>
#include <unistd.h>

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

    msgid = msgget(MSG_KEY, IPC_CREAT | 0666);
    if (msgid == -1)
    {
        perror("msgget");
        exit(1);
    }

    msg.mtype = 1;
    strcpy(msg.mtext, "Hi!");
    if (msgsnd(msgid, &msg, strlen(msg.mtext)+1, 0) == -1)
    {
        perror("msgsnd");
        msgctl(msgid, IPC_RMID, NULL);
        exit(1);
    }
    printf("Server: Sent message: %s\n", msg.mtext);

    if (msgrcv(msgid, &msg, MAX_TEXT, 2, 0) == -1)
    {
        perror("msgrcv");
        msgctl(msgid, IPC_RMID, NULL);
        exit(1);
    }
    printf("Server: Received message: %s\n", msg.mtext);

    if (msgctl(msgid, IPC_RMID, NULL) == -1)
    {
        perror("msgctl");
        exit(1);
    }

    return 0;
}
