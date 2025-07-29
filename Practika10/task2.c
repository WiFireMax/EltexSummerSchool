#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

void make_children(int num)
{
    for (int i = 0; i < num; ++i)
    {
        pid_t pid = fork();
        if (pid < 0)
        {
            perror("fork");
            exit(1);
        } else if (pid == 0) {
            printf("Process: pid=%d, ppid=%d\n", getpid(), getppid());
            return;
        }
    }
    for (int i = 0; i < num; ++i)
        wait(NULL);
}

int main()
{
    pid_t pid1 = fork();
    if (pid1 == 0)
    {
        printf("Process1: pid=%d, ppid=%d\n", getpid(), getppid());
        make_children(2);
        exit(0);
    }

    pid_t pid2 = fork();
    if (pid2 == 0)
    {
        printf("Process2: pid=%d, ppid=%d\n", getpid(), getppid());
        make_children(1);
        exit(0);
    }

    printf("Parent: pid=%d, ppid=%d\n", getpid(), getppid());
    for (int i = 0; i < 2; ++i)
        wait(NULL);

    return 0;
}
