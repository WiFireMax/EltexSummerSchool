#ifndef _CHAT_COMMON_H_
#define _CHAT_COMMON_H_

#include <pthread.h>

#define MAX_CLIENTS 10
#define MAX_NAME_LEN 32
#define MAX_MSG_LEN 128
#define MAX_MESSAGES 100

typedef struct
{
    char sender[MAX_NAME_LEN];
    char message[MAX_MSG_LEN];
} ChatMessage;

typedef struct
{
    char clients[MAX_CLIENTS][MAX_NAME_LEN];
    int client_count;

    ChatMessage messages[MAX_MESSAGES];
    int msg_start;
    int msg_count;

    pthread_mutex_t mutex;
    pthread_cond_t cond;
} ChatRoom;

#define SHM_NAME "/chat_shm"

#endif
