#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>

void sigusr1_handler(int signo)
{
    printf("Получен сигнал SIGUSR1!\n");
}

int main() {
    struct sigaction sa;

    sa.sa_handler = sigusr1_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;

    if (sigaction(SIGUSR1, &sa, NULL) == -1)
    {
        perror("sigaction");
        exit(1);
    }

    printf("PID: %d - Ожидание сигнала SIGUSR1...\n", getpid());

    while (1)
    {
        pause();
    }

    return 0;
}
