#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <semaphore.h>
#include <unistd.h>
#include <string.h>

#define SHM_NAME "/my_shm"
#define SEM_SERVER_TO_CLIENT "/sem_s2c"
#define SEM_CLIENT_TO_SERVER "/sem_c2s"
#define MSG_SIZE 64

int main()
{
    int shm_fd = shm_open(SHM_NAME, O_CREAT | O_RDWR, 0600);
    if (shm_fd < 0)
    {
        perror("shm_open");
        exit(1);
    }

    if (ftruncate(shm_fd, MSG_SIZE) == -1)
    {
        perror("ftruncate");
        exit(1);
    }

    char *shm_ptr = mmap(NULL, MSG_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    if (shm_ptr == MAP_FAILED)
    {
        perror("mmap");
        exit(1);
    }

    sem_t *sem_s2c = sem_open(SEM_SERVER_TO_CLIENT, O_CREAT, 0600, 0);
    if (sem_s2c == SEM_FAILED)
    {
        perror("sem_open sem_s2c");
        exit(1);
    }
    sem_t *sem_c2s = sem_open(SEM_CLIENT_TO_SERVER, O_CREAT, 0600, 0);
    if (sem_c2s == SEM_FAILED)
    {
        perror("sem_open sem_c2s");
        exit(1);
    }

    strncpy(shm_ptr, "Hi!", MSG_SIZE);
    printf("Server: wrote message: %s\n", shm_ptr);

    sem_post(sem_s2c);

    sem_wait(sem_c2s);

    printf("Server: received message: %s\n", shm_ptr);

    munmap(shm_ptr, MSG_SIZE);
    close(shm_fd);
    shm_unlink(SHM_NAME);

    sem_close(sem_s2c);
    sem_close(sem_c2s);
    sem_unlink(SEM_SERVER_TO_CLIENT);
    sem_unlink(SEM_CLIENT_TO_SERVER);

    return 0;
}
