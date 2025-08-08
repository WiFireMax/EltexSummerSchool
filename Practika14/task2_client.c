#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>
#include <pthread.h>
#include <ncurses.h>
#include "chat_common.h"


ChatRoom *chat;
char my_name[MAX_NAME_LEN];

WINDOW *win_chat, *win_input;
pthread_mutex_t ncurses_mutex = PTHREAD_MUTEX_INITIALIZER;


void print_all_messages()
{
    pthread_mutex_lock(&ncurses_mutex);

    werase(win_chat);
    box(win_chat, 0, 0);

    int max_lines, max_cols;
    getmaxyx(win_chat, max_lines, max_cols);
    int lines_for_text = max_lines - 2;

    int msgs_to_show = (chat->msg_count < lines_for_text) ? chat->msg_count : lines_for_text;
    int first_msg_idx = (chat->msg_count <= MAX_MESSAGES) ? (chat->msg_count - msgs_to_show) : (chat->msg_count - msgs_to_show);

    for (int i = 0; i < msgs_to_show; i++)
    {
        int msg_num = first_msg_idx + i;
        int idx = (chat->msg_start + msg_num) % MAX_MESSAGES;

        char line_buf[MAX_NAME_LEN + MAX_MSG_LEN + 4];
        snprintf(line_buf, sizeof(line_buf), "%s: %s",
                 chat->messages[idx].sender,
                 chat->messages[idx].message);

        mvwprintw(win_chat, i + 1, 1, "%.*s", max_cols - 2, line_buf);
    }
    wrefresh(win_chat);

    pthread_mutex_unlock(&ncurses_mutex);
}

void remove_myself_from_clients()
{
    pthread_mutex_lock(&chat->mutex);

    int idx = -1;
    for (int i = 0; i < chat->client_count; i++)
    {
        if (strncmp(chat->clients[i], my_name, MAX_NAME_LEN) == 0)
        {
            idx = i;
            break;
        }
    }
    if (idx != -1)
    {
        for (int i = idx; i < chat->client_count - 1; i++)
        {
            strncpy(chat->clients[i], chat->clients[i + 1], MAX_NAME_LEN);
        }
        chat->client_count--;

        int pos = (chat->msg_start + chat->msg_count) % MAX_MESSAGES;
        strncpy(chat->messages[pos].sender, "<System>", MAX_NAME_LEN - 1);
        chat->messages[pos].sender[MAX_NAME_LEN - 1] = '\0';
        snprintf(chat->messages[pos].message, MAX_MSG_LEN, "User %s left the chat", my_name);

        if (chat->msg_count < MAX_MESSAGES)
        {
            chat->msg_count++;
        } else {
            chat->msg_start = (chat->msg_start + 1) % MAX_MESSAGES;
        }
        pthread_cond_broadcast(&chat->cond);
    }

    pthread_mutex_unlock(&chat->mutex);
}

void *receive_thread(void *arg) {
    int last_msg_count = 0;

    pthread_mutex_lock(&chat->mutex);
    while (1)
    {
        while (chat->msg_count == last_msg_count)
        {
            pthread_cond_wait(&chat->cond, &chat->mutex);
        }
        print_all_messages();
        last_msg_count = chat->msg_count;
    }
    pthread_mutex_unlock(&chat->mutex);
    return NULL;
}

int main() {
    printf("Enter your name: ");
    if (fgets(my_name, MAX_NAME_LEN, stdin) == NULL)
    {
        perror("fgets");
        exit(EXIT_FAILURE);
    }
    my_name[strcspn(my_name, "\n")] = '\0';

    int shm_fd = shm_open(SHM_NAME, O_RDWR, 0);
    if (shm_fd == -1)
    {
        perror("shm_open");
        exit(EXIT_FAILURE);
    }

    chat = mmap(NULL, sizeof(ChatRoom), PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    if (chat == MAP_FAILED)
    {
        perror("mmap");
        exit(EXIT_FAILURE);
    }

    pthread_mutex_lock(&chat->mutex);
    if (chat->client_count < MAX_CLIENTS)
    {
        strncpy(chat->clients[chat->client_count], my_name, MAX_NAME_LEN - 1);
        chat->clients[chat->client_count][MAX_NAME_LEN - 1] = '\0';
        chat->client_count++;

        int pos = (chat->msg_start + chat->msg_count) % MAX_MESSAGES;
        strncpy(chat->messages[pos].sender, "<System>", MAX_NAME_LEN - 1);
        chat->messages[pos].sender[MAX_NAME_LEN - 1] = '\0';
        snprintf(chat->messages[pos].message, MAX_MSG_LEN, "User %s joined the chat", my_name);

        if (chat->msg_count < MAX_MESSAGES)
        {
            chat->msg_count++;
        } else {
            chat->msg_start = (chat->msg_start + 1) % MAX_MESSAGES;
        }
        pthread_cond_broadcast(&chat->cond);
    } else {
        pthread_mutex_unlock(&chat->mutex);
        fprintf(stderr, "Chat room is full\n");
        exit(EXIT_FAILURE);
    }
    pthread_mutex_unlock(&chat->mutex);

    initscr();
    cbreak();
    noecho();
    keypad(stdscr, TRUE);
    curs_set(1);

    int rows, cols;
    getmaxyx(stdscr, rows, cols);

    int input_height = 3;
    int chat_height = rows - input_height;

    win_chat = newwin(chat_height, cols, 0, 0);
    win_input = newwin(input_height, cols, chat_height, 0);

    scrollok(win_chat, TRUE);

    box(win_chat, 0, 0);
    box(win_input, 0, 0);

    mvwprintw(win_input, 1, 1, "Type message (type /quit to exit): ");
    wrefresh(win_chat);
    wrefresh(win_input);

    pthread_t thr;
    pthread_create(&thr, NULL, receive_thread, NULL);

    char input_buf[MAX_MSG_LEN];

    while (1)
    {
        pthread_mutex_lock(&ncurses_mutex);
        werase(win_input);
        box(win_input, 0, 0);
        mvwprintw(win_input, 1, 1, "Type message (type /quit to exit): ");
        wrefresh(win_input);
        pthread_mutex_unlock(&ncurses_mutex);

        echo();
        wgetnstr(win_input, input_buf, MAX_MSG_LEN - 1);
        noecho();

        if (strcmp(input_buf, "/quit") == 0)
        {
            remove_myself_from_clients();
            break;
        }

        if (strlen(input_buf) == 0)
        {
            continue;
        }

        pthread_mutex_lock(&chat->mutex);
        int pos = (chat->msg_start + chat->msg_count) % MAX_MESSAGES;
        strncpy(chat->messages[pos].sender, my_name, MAX_NAME_LEN - 1);
        chat->messages[pos].sender[MAX_NAME_LEN - 1] = '\0';
        strncpy(chat->messages[pos].message, input_buf, MAX_MSG_LEN - 1);
        chat->messages[pos].message[MAX_MSG_LEN - 1] = '\0';

        if (chat->msg_count < MAX_MESSAGES)
        {
            chat->msg_count++;
        } else {
            chat->msg_start = (chat->msg_start + 1) % MAX_MESSAGES;
        }

        pthread_cond_broadcast(&chat->cond);
        pthread_mutex_unlock(&chat->mutex);
    }

    endwin();

    return 0;
}
