#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

#define SHM_SIZE 64

union semun
{
    int val;
    struct semid_ds *buf;
    unsigned short *array;
};

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

    int shmid = shmget(key, SHM_SIZE, IPC_CREAT | 0666);
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

    int semid = semget(key, 2, IPC_CREAT | 0666);
    if (semid == -1)
    {
        perror("semget");
        exit(1);
    }

    union semun arg;
    arg.val = 0;
    if (semctl(semid, 0, SETVAL, arg) == -1)
    {
        perror("semctl 0");
        exit(1);
    }
    if (semctl(semid, 1, SETVAL, arg) == -1)
    {
        perror("semctl 1");
        exit(1);
    }

    strncpy(shm_ptr, "Hi!", SHM_SIZE);
    printf("Server: wrote message: %s\n", shm_ptr);

    sem_op(semid, 0, 1);

    sem_op(semid, 1, -1);

    printf("Server: received message: %s\n", shm_ptr);

    shmdt(shm_ptr);
    shmctl(shmid, IPC_RMID, NULL);
    semctl(semid, 0, IPC_RMID);

    return 0;
}
