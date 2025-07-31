#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <string.h>

#define FIFO_NAME "myfifo"

int main()
{
    mkfifo(FIFO_NAME, 0666);
    int fd = open(FIFO_NAME, O_WRONLY);
    const char *msg = "Hi!";
    write(fd, msg, strlen(msg));
    close(fd);
    return 0;
}
