#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>

int main()
{
    sigset_t waitset;
    int sig;

    sigemptyset(&waitset);
    sigaddset(&waitset, SIGUSR1);

    if (sigprocmask(SIG_BLOCK, &waitset, NULL) == -1)
    {
        perror("sigprocmask");
        exit(1);
    }

    printf("Ожидание сигнала SIGUSR1. PID: %d\n", getpid());

    while (1)
    {
        if (sigwait(&waitset, &sig) == 0)
        {
            if (sig == SIGUSR1)
            {
                printf("Получен сигнал SIGUSR1 от sigwait().\n");
            }
        } else {
            perror("sigwait");
            exit(1);
        }
    }

    return 0;
}
