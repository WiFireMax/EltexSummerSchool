#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>
#include <pthread.h>
#include "chat_common.h"


int main()
{
    int shm_fd = shm_open(SHM_NAME, O_CREAT | O_RDWR, 0600);
    if (shm_fd == -1)
    {
        perror("shm_open");
        exit(1);
    }
    if (ftruncate(shm_fd, sizeof(ChatRoom)) == -1)
    {
        perror("ftruncate");
        exit(1);
    }

    ChatRoom *chat = mmap(NULL, sizeof(ChatRoom), PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    if (chat == MAP_FAILED)
    {
        perror("mmap");
        exit(1);
    }

    chat->client_count = 0;
    chat->msg_start = 0;
    chat->msg_count = 0;
    pthread_mutexattr_t mattr;
    pthread_condattr_t cattr;
    pthread_mutexattr_init(&mattr);
    pthread_mutexattr_setpshared(&mattr, PTHREAD_PROCESS_SHARED);
    pthread_condattr_init(&cattr);
    pthread_condattr_setpshared(&cattr, PTHREAD_PROCESS_SHARED);

    pthread_mutex_init(&chat->mutex, &mattr);
    pthread_cond_init(&chat->cond, &cattr);

    printf("Chat server started. Waiting for clients...\n");

    while (1)
    {
        pthread_mutex_lock(&chat->mutex);
        int clients = chat->client_count;
        int messages = chat->msg_count;
        pthread_mutex_unlock(&chat->mutex);
        printf("Active clients: %d, messages: %d\n", clients, messages);
        sleep(5);
    }
    return 0;
}
