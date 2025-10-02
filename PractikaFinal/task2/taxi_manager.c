#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <poll.h>
#include <sys/timerfd.h>
#include <fcntl.h>
#include <stdint.h>
#include <signal.h>
#include <errno.h>
#include <sys/wait.h>

#define MAX_DRIVERS 100
#define BUF_SIZE 256

typedef enum { AVAILABLE, BUSY } status_t;

typedef struct
{
    pid_t pid;
    int fd;
    status_t status;
    int busy_time_left;
} driver_t;

driver_t drivers[MAX_DRIVERS];
int drivers_count = 0;

int find_driver_index(pid_t pid)
{
    for (int i = 0; i < drivers_count; i++)
    {
        if (drivers[i].pid == pid)
            return i;
    }
    return -1;
}

int send_command(int fd, const char *cmd, char *resp, size_t resp_size)
{
    ssize_t w = write(fd, cmd, strlen(cmd));
    if (w < 0) return -1;
    ssize_t r = read(fd, resp, resp_size - 1);
    if (r < 0) return -1;
    resp[r] = 0;
    return 0;
}

void driver_process(int fd)
{
    status_t status = AVAILABLE;
    int timer_fd = -1;
    int busy_time_left = 0;

    struct pollfd fds[2];
    char buf[BUF_SIZE];

    while (1)
    {
        if (status == BUSY && timer_fd < 0)
        {
            status = AVAILABLE;
            busy_time_left = 0;
        }

        int fds_count = 1;
        fds[0].fd = fd;
        fds[0].events = POLLIN;
        if (status == BUSY)
        {
            if (timer_fd < 0)
            {
                timer_fd = timerfd_create(CLOCK_MONOTONIC, TFD_NONBLOCK);
                struct itimerspec its = {0};
                its.it_value.tv_sec = busy_time_left;
                timerfd_settime(timer_fd, 0, &its, NULL);
            }
            fds[1].fd = timer_fd;
            fds[1].events = POLLIN;
            fds_count = 2;
        }

        int ret = poll(fds, fds_count, -1);
        if (ret < 0)
        {
            if (errno == EINTR) continue;
            perror("poll");
            break;
        }

        if (fds[0].revents & POLLIN)
        {
            ssize_t n = read(fd, buf, sizeof(buf) - 1);
            if (n <= 0)
            {
                break;
            }
            buf[n] = 0;

            if (strncmp(buf, "start ", 6) == 0)
            {
                int task_time = atoi(buf + 6);
                if (status == BUSY)
                {
                    dprintf(fd, "Busy %d\n", busy_time_left);
                } else
                {
                    if (timer_fd >= 0) close(timer_fd);
                    timer_fd = timerfd_create(CLOCK_MONOTONIC, TFD_NONBLOCK);
                    struct itimerspec its = {0};
                    its.it_value.tv_sec = task_time;
                    timerfd_settime(timer_fd, 0, &its, NULL);
                    status = BUSY;
                    busy_time_left = task_time;
                    dprintf(fd, "OK\n");
                }
            } else if (strncmp(buf, "status", 6) == 0)
            {
                if (status == AVAILABLE)
                    dprintf(fd, "Available\n");
                else
                    dprintf(fd, "Busy %d\n", busy_time_left);
            } else {
                dprintf(fd, "Unknown command\n");
            }
        }

        if (status == BUSY && (fds_count == 2) && (fds[1].revents & POLLIN))
        {
            uint64_t expirations;
            ssize_t s = read(timer_fd, &expirations, sizeof(expirations));
            if (s != sizeof(expirations))
            {
                perror("timerfd read");
                break;
            }
            status = AVAILABLE;
            busy_time_left = 0;
            close(timer_fd);
            timer_fd = -1;
            dprintf(fd, "Available\n");
        }
    }

    if (timer_fd >= 0) close(timer_fd);
    close(fd);
    exit(0);
}

void cmd_create_driver()
{
    if (drivers_count >= MAX_DRIVERS)
    {
        printf("Max drivers reached\n");
        return;
    }
    int sv[2];
    if (socketpair(AF_UNIX, SOCK_STREAM, 0, sv) < 0)
    {
        perror("socketpair");
        return;
    }
    pid_t pid = fork();
    if (pid < 0)
    {
        perror("fork");
        close(sv[0]);
        close(sv[1]);
        return;
    }
    if (pid == 0)
    {
        close(sv[0]);
        driver_process(sv[1]);
    }
    close(sv[1]);
    drivers[drivers_count].pid = pid;
    drivers[drivers_count].fd = sv[0];
    drivers[drivers_count].status = AVAILABLE;
    drivers[drivers_count].busy_time_left = 0;
    drivers_count++;
    printf("Created driver with pid %d\n", pid);
}

void cmd_send_task(char *arg1, char *arg2)
{
    pid_t pid = atoi(arg1);
    int task_time = atoi(arg2);
    int idx = find_driver_index(pid);
    if (idx < 0)
    {
        printf("Driver pid %d not found\n", pid);
        return;
    }
    char cmd[BUF_SIZE];
    snprintf(cmd, sizeof(cmd), "start %d\n", task_time);
    char resp[BUF_SIZE];
    if (send_command(drivers[idx].fd, cmd, resp, sizeof(resp)) < 0)
    {
        printf("Failed to send command to driver\n");
        return;
    }
    if (strncmp(resp, "OK", 2) == 0)
    {
        drivers[idx].status = BUSY;
        drivers[idx].busy_time_left = task_time;
        printf("Task sent: driver %d busy for %d seconds\n", pid, task_time);
    } else if (strncmp(resp, "Busy", 4) == 0)
    {
        int left = 0;
        sscanf(resp, "Busy %d", &left);
        drivers[idx].status = BUSY;
        drivers[idx].busy_time_left = left;
        printf("Driver %d busy %d seconds left\n", pid, left);
    } else {
        printf("Driver response: %s\n", resp);
    }
}

void cmd_get_status(char *arg)
{
    pid_t pid = atoi(arg);
    int idx = find_driver_index(pid);
    if (idx < 0)
    {
        printf("Driver %d not found\n", pid);
        return;
    }
    char resp[BUF_SIZE];
    if (send_command(drivers[idx].fd, "status\n", resp, sizeof(resp)) < 0)
    {
        printf("Failed to contact driver\n");
        return;
    }
    printf("Driver %d status: %s", pid, resp);
    if (strncmp(resp, "Busy", 4) == 0)
    {
        int left = 0;
        sscanf(resp, "Busy %d", &left);
        drivers[idx].status = BUSY;
        drivers[idx].busy_time_left = left;
    } else if (strncmp(resp, "Available", 9) == 0)
    {
        drivers[idx].status = AVAILABLE;
        drivers[idx].busy_time_left = 0;
    }
}

void cmd_get_drivers()
{
    printf("Drivers (%d):\n", drivers_count);
    for (int i = 0; i < drivers_count; i++)
    {
        printf("  pid %d: %s", drivers[i].pid,
            drivers[i].status == AVAILABLE ? "Available\n" :
            "Busy\n");
    }
}

void read_driver_messages() {
    for (int i = 0; i < drivers_count; i++)
    {
        char buf[BUF_SIZE];
        ssize_t n = read(drivers[i].fd, buf, sizeof(buf) - 1);
        if (n > 0)
        {
            buf[n] = 0;
            if (strncmp(buf, "Available", 9) == 0)
            {
                drivers[i].status = AVAILABLE;
                drivers[i].busy_time_left = 0;
                printf("Driver %d now Available\n", drivers[i].pid);
            }
            else if (strncmp(buf, "Busy ", 5) == 0)
            {
                int left = 0;
                sscanf(buf, "Busy %d", &left);
                drivers[i].status = BUSY;
                drivers[i].busy_time_left = left;
            }
        }
    }
}

void reap_zombies()
{
    while (waitpid(-1, NULL, WNOHANG) > 0) {}
}

int main()
{
    char line[BUF_SIZE];
    struct pollfd fds[MAX_DRIVERS + 1];

    printf("Taxi manager CLI ready. Commands:\n");
    printf("create_driver\nsend_task <pid> <time>\nget_status <pid>\nget_drivers\nexit\n");

    while (1)
    {
        reap_zombies();

        fds[0].fd = STDIN_FILENO;
        fds[0].events = POLLIN;

        for (int i = 0; i < drivers_count; i++)
        {
            fds[i + 1].fd = drivers[i].fd;
            fds[i + 1].events = POLLIN;
        }

        int ret = poll(fds, drivers_count + 1, -1);
        if (ret < 0)
        {
            if (errno == EINTR) continue;
            perror("poll");
            break;
        }

        if (fds[0].revents & POLLIN)
        {
            if (!fgets(line, sizeof(line), stdin))
            {
                break;
            }
            char *cmd = strtok(line, " \t\n");
            if (!cmd) continue;

            if (strcmp(cmd, "exit") == 0)
            {
                break;
            } else if (strcmp(cmd, "create_driver") == 0)
            {
                cmd_create_driver();
            } else if (strcmp(cmd, "send_task") == 0)
            {
                char *p1 = strtok(NULL, " \t\n");
                char *p2 = strtok(NULL, " \t\n");
                if (!p1 || !p2)
                {
                    printf("Usage: send_task <pid> <time>\n");
                    continue;
                }
                cmd_send_task(p1, p2);
            } else if (strcmp(cmd, "get_status") == 0)
            {
                char *p1 = strtok(NULL, " \t\n");
                if (!p1)
                {
                    printf("Usage: get_status <pid>\n");
                    continue;
                }
                cmd_get_status(p1);
            } else if (strcmp(cmd, "get_drivers") == 0)
            {
                cmd_get_drivers();
            } else {
                printf("Unknown command\n");
            }
        }

        for (int i = 0; i < drivers_count; i++)
        {
            if (fds[i+1].revents & POLLIN)
            {
                read_driver_messages();
                break;
            }
        }
    }

    for (int i = 0; i < drivers_count; i++)
    {
        kill(drivers[i].pid, SIGTERM);
        close(drivers[i].fd);
    }
    while(wait(NULL) > 0);

    printf("Exited.\n");
    return 0;
}
