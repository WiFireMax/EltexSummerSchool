#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        fprintf(stderr, "Шаблон использования: %s <PID>\n", argv[0]);
        return 1;
    }

    pid_t pid = (pid_t)atoi(argv[1]);

    if (kill(pid, SIGUSR1) == -1)
    {
        perror("kill");
        return 1;
    }

    printf("Сигнал SIGUSR1 отправлен процессу %d\n", pid);

    return 0;
}
