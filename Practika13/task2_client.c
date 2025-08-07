#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <string.h>
#include <pthread.h>
#include <ncurses.h>

#define SERVER_KEY 0x1234

struct chat_msg
{
    long mtype;
    pid_t pid;
    char name[32];
    char text[256];
};

int server_queue_id, client_queue_id;
pid_t pid;
char client_name[32];

WINDOW *chatwin, *inputwin;

pthread_mutex_t screen_mutex = PTHREAD_MUTEX_INITIALIZER;

void *recv_thread_func(void *arg)
{
    struct chat_msg msg;
    while (1)
    {
        ssize_t rcv = msgrcv(client_queue_id, &msg, sizeof(struct chat_msg) - sizeof(long), 0, 0);
        if (rcv == -1)
        {
            perror("msgrcv in client");
            continue;
        }
        pthread_mutex_lock(&screen_mutex);
        wprintw(chatwin, "[%s]: %s\n", msg.name, msg.text);
        wrefresh(chatwin);
        pthread_mutex_unlock(&screen_mutex);
    }
    return NULL;
}

int main()
{
    initscr();
    cbreak();
    noecho();

    int rows, cols;
    getmaxyx(stdscr, rows, cols);

    chatwin = newwin(rows - 3, cols, 0, 0);
    inputwin = newwin(3, cols, rows - 3, 0);
    scrollok(chatwin, TRUE);
    box(inputwin, 0, 0);

    wrefresh(chatwin);
    wrefresh(inputwin);

    pthread_mutex_lock(&screen_mutex);
    mvwprintw(inputwin, 1, 1, "Enter your name: ");
    wrefresh(inputwin);
    echo();
    wgetnstr(inputwin, client_name, sizeof(client_name) - 1);
    noecho();
    werase(inputwin);
    box(inputwin, 0, 0);
    wrefresh(inputwin);
    pthread_mutex_unlock(&screen_mutex);

    pid = getpid();

    server_queue_id = msgget(SERVER_KEY, 0666);
    if (server_queue_id == -1)
    {
        endwin();
        perror("msgget server");
        exit(1);
    }

    key_t client_key = SERVER_KEY + pid;
    client_queue_id = msgget(client_key, IPC_CREAT | 0666);
    if (client_queue_id == -1)
    {
        endwin();
        perror("msgget client");
        exit(1);
    }

    struct chat_msg msg;
    msg.mtype = 1;
    msg.pid = pid;
    strncpy(msg.name, client_name, sizeof(msg.name)-1);
    msg.name[sizeof(msg.name)-1] = 0;
    msg.text[0] = 0;

    if (msgsnd(server_queue_id, &msg, sizeof(struct chat_msg) - sizeof(long), 0) == -1)
    {
        endwin();
        perror("msgsnd login");
        exit(1);
    }

    pthread_t recv_thread;
    pthread_create(&recv_thread, NULL, recv_thread_func, NULL);

    char input_buf[256];
    while (1)
    {
        pthread_mutex_lock(&screen_mutex);
        werase(inputwin);
        box(inputwin, 0, 0);
        mvwprintw(inputwin, 1, 1, "Your message ('/quit' for exit): ");
        wrefresh(inputwin);
        pthread_mutex_unlock(&screen_mutex);

        echo();
        wgetnstr(inputwin, input_buf, sizeof(input_buf)-1);
        noecho();

        if (strncmp(input_buf, "/quit", 5) == 0) break;

        msg.mtype = 2;
        msg.pid = pid;
        strncpy(msg.text, input_buf, sizeof(msg.text)-1);
        msg.text[sizeof(msg.text)-1] = 0;

        if (msgsnd(server_queue_id, &msg, sizeof(struct chat_msg) - sizeof(long), 0) == -1)
        {
            pthread_mutex_lock(&screen_mutex);
            wprintw(chatwin, "Failed to send message\n");
            wrefresh(chatwin);
            pthread_mutex_unlock(&screen_mutex);
        }
    }

    msgctl(client_queue_id, IPC_RMID, NULL);

    pthread_cancel(recv_thread);
    pthread_join(recv_thread, NULL);

    endwin();

    return 0;
}
