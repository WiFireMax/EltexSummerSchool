#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>

#define FIFO_NAME "myfifo"

int main() {
    int fd = open(FIFO_NAME, O_RDONLY);
    char buf[100];
    ssize_t n = read(fd, buf, sizeof(buf)-1);
    if (n > 0)
    {
        buf[n] = '\0';
        printf("Client received: %s\n", buf);
    }
    close(fd);
    unlink(FIFO_NAME);
    return 0;
}
