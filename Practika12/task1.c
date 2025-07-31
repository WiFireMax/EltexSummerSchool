#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>

int main()
{
    int fd[2];
    pipe(fd);
    pid_t pid = fork();

    if (pid < 0)
    {
        perror("fork");
        exit(1);
    } else if (pid == 0) {
        close(fd[1]);
        char buf[100];
        ssize_t n = read(fd[0], buf, sizeof(buf)-1);
        if (n > 0)
        {
            buf[n] = '\0';
            printf("Child received: %s\n", buf);
        }
        close(fd[0]);
        exit(0);
    } else {
        close(fd[0]);
        const char *msg = "Hi!";
        write(fd[1], msg, strlen(msg));
        close(fd[1]);
        waitpid(pid, NULL, 0);
    }
    return 0;
}
