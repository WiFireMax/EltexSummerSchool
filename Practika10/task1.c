#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main()
{
    pid_t pid = fork();

    if (pid < 0)
    {
        perror("fork error");
        exit(1);
    } else if (pid == 0) {
        printf("Child process: pid=%d, ppid=%d\n", getpid(), getppid());
        exit(42);
    } else {
        int status;
        printf("Parent process: pid=%d, ppid=%d\n", getpid(), getppid());
        waitpid(pid, &status, 0);
        if (WIFEXITED(status))
        {
            printf("Child exited with status=%d\n", WEXITSTATUS(status));
        }
    }
    return 0;
}
