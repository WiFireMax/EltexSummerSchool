#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

#define MAX_LINE 1024
#define MAX_ARGS 100

void parse_command(char *line, char **args)
{
    int argc = 0;
    char *token = strtok(line, " ");
    while (token && argc < MAX_ARGS - 1)
    {
        args[argc++] = token;
        token = strtok(NULL, " ");
    }
    args[argc] = NULL;
}

int main()
{
    char line[MAX_LINE];

    while (1) {
        printf("mysh$ ");
        if (!fgets(line, sizeof(line), stdin))
            break;
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "exit") == 0)
            break;

        char *pipe_pos = strchr(line, '|');
        if (!pipe_pos)
        {
            char *args[MAX_ARGS];
            parse_command(line, args);
            if (args[0] == NULL) continue;

            pid_t pid = fork();
            if (pid == 0)
            {
                execvp(args[0], args);
                perror("execvp");
                exit(1);
            }
            waitpid(pid, NULL, 0);
        } else {
            *pipe_pos = '\0';
            char *cmd1 = line;
            char *cmd2 = pipe_pos + 1;
            while (*cmd2 == ' ') cmd2++;

            char *args1[MAX_ARGS], *args2[MAX_ARGS];
            parse_command(cmd1, args1);
            parse_command(cmd2, args2);

            int fd[2];
            pipe(fd);
            pid_t pid1 = fork();
            if (pid1 == 0)
            {
                close(fd[0]);
                dup2(fd[1], STDOUT_FILENO);
                close(fd[1]);
                execvp(args1[0], args1);
                perror("execvp 1");
                exit(1);
            }

            pid_t pid2 = fork();
            if (pid2 == 0)
            {
                close(fd[1]);
                dup2(fd[0], STDIN_FILENO);
                close(fd[0]);
                execvp(args2[0], args2);
                perror("execvp 2");
                exit(1);
            }

            close(fd[0]);
            close(fd[1]);
            waitpid(pid1, NULL, 0);
            waitpid(pid2, NULL, 0);
        }
    }

    return 0;
}
