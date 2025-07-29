#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

#define MAX_CMD 1024
#define MAX_ARGS 100

int main()
{
    char line[MAX_CMD];
    char *args[MAX_ARGS];

    while (1)
    {
        printf("mysh$ ");
        if (!fgets(line, sizeof(line), stdin))
            break;

        line[strcspn(line, "\n")] = 0;

        if (strcmp(line, "exit") == 0)
            break;

        int argc = 0;
        char *token = strtok(line, " ");
        while (token && argc < MAX_ARGS - 1)
        {
            args[argc++] = token;
            token = strtok(NULL, " ");
        }
        args[argc] = NULL;
        if (argc == 0)
            continue;

        pid_t pid = fork();
        if (pid == 0)
        {
            execvp(args[0], args);
            perror("execvp error");
            exit(1);
        } else if (pid > 0) {
            waitpid(pid, NULL, 0);
        } else {
            perror("fork error");
        }
    }
    return 0;
}
