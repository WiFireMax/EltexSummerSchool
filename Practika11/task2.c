#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

#define N_THREADS 8
#define N_ITERS 100000

long counter = 0;
pthread_mutex_t lock;

void* increment_no_mutex(void* arg)
{
    for (int i = 0; i < N_ITERS; ++i)
        counter++;
    return NULL;
}

void* increment_with_mutex(void* arg)
{
    for (int i = 0; i < N_ITERS; ++i)
    {
        pthread_mutex_lock(&lock);
        counter++;
        pthread_mutex_unlock(&lock);
    }
    return NULL;
}

int main()
{
    pthread_t threads[N_THREADS];

    counter = 0;
    for (int i = 0; i < N_THREADS; ++i)
        pthread_create(&threads[i], NULL, increment_no_mutex, NULL);
    for (int i = 0; i < N_THREADS; ++i)
        pthread_join(threads[i], NULL);
    printf("Результат без mutex(конкуренция):   %ld\n", counter);

    counter = 0;
    pthread_mutex_init(&lock, NULL);
    for (int i = 0; i < N_THREADS; ++i)
        pthread_create(&threads[i], NULL, increment_with_mutex, NULL);
    for (int i = 0; i < N_THREADS; ++i)
        pthread_join(threads[i], NULL);
    printf("Результат с mutex:   %ld\n", counter);
    pthread_mutex_destroy(&lock);

    return 0;
}
