#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <unistd.h>
#include <string.h>

#define SHM_SIZE 64

void sem_op(int semid, int sem_num, int op)
{
    struct sembuf sb = {sem_num, op, 0};
    if (semop(semid, &sb, 1) == -1)
    {
        perror("semop");
        exit(1);
    }
}

int main()
{
    key_t key = ftok(".", 'S');
    if (key == -1)
    {
        perror("ftok");
        exit(1);
    }

    int shmid = shmget(key, SHM_SIZE, 0666);
    if (shmid == -1)
    {
        perror("shmget");
        exit(1);
    }

    char *shm_ptr = (char*)shmat(shmid, NULL, 0);
    if (shm_ptr == (char*)-1)
    {
        perror("shmat");
        exit(1);
    }

    int semid = semget(key, 2, 0666);
    if (semid == -1)
    {
        perror("semget");
        exit(1);
    }

    sem_op(semid, 0, -1);

    printf("Client: received message: %s\n", shm_ptr);

    strncpy(shm_ptr, "Hello!", SHM_SIZE);

    sem_op(semid, 1, 1);

    shmdt(shm_ptr);

    return 0;
}
