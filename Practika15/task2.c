#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>

int main()
{
    sigset_t newset;

    sigemptyset(&newset);
    sigaddset(&newset, SIGINT);

    if (sigprocmask(SIG_BLOCK, &newset, NULL) == -1)
    {
        perror("sigprocmask");
        exit(1);
    }

    printf("SIGINT заблокирован. PID: %d.", getpid());

    while (1)
    {
        pause();
    }

    return 0;
}

