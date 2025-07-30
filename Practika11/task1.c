#include <stdio.h>
#include <pthread.h>

void* thread_func(void* arg)
{
    int idx = *((int*)arg);
    printf("Thread #%d start\n", idx);
    return NULL;
}

int main()
{
    pthread_t threads[5];
    int indices[5];
    for (int i = 0; i < 5; ++i)
    {
        indices[i] = i;
        pthread_create(&threads[i], NULL, thread_func, &indices[i]);
    }
    for (int i = 0; i < 5; ++i)
        pthread_join(threads[i], NULL);
    return 0;
}
