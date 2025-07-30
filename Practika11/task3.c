#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <stdbool.h>
#include <time.h>
#include <stdarg.h>

#define NUM_STORES 5
#define NUM_BUYERS 3
#define BUYER_MAX_NEED 100000
#define STORE_MAX_INIT 10000
#define LOADER_ADD 5000

typedef struct
{
    int products;
    pthread_mutex_t mutex;
    bool occupied;
} store_t;

store_t stores[NUM_STORES];
FILE* log_file;
pthread_mutex_t log_mutex = PTHREAD_MUTEX_INITIALIZER;
bool buyers_done = false;

void log_msg(const char* fmt, ...)
{
    pthread_mutex_lock(&log_mutex);
    va_list args;
    va_start(args, fmt);
    vfprintf(log_file, fmt, args);
    fflush(log_file);
    va_end(args);
    pthread_mutex_unlock(&log_mutex);
}

void* buyer_func(void* arg)
{
    int id = *((int*)arg);
    int need = rand() % BUYER_MAX_NEED + 1;
    log_msg("Покупатель %d начал покупки, ему нужно %d\n", id, need);

    while (need > 0)
    {
        for (int i = 0; i < NUM_STORES; ++i)
        {
            if (pthread_mutex_trylock(&stores[i].mutex) == 0)
            {
                if (!stores[i].occupied && stores[i].products > 0)
                {
                    stores[i].occupied = true;
                    int taken = (need < stores[i].products) ? need : stores[i].products;
                    stores[i].products -= taken;
                    need -= taken;
                    log_msg("Покупатель %d в магазине %d: взял %d, еще нужно %d, в магазине осталось: %d\n", id, i, taken, need, stores[i].products);
                    stores[i].occupied = false;
                    pthread_mutex_unlock(&stores[i].mutex);
                    if (need > 0) {
                        log_msg("Покупатель %d спит 2 секунды\n", id);
                        sleep(2);
                    }
                    break;
                }
                pthread_mutex_unlock(&stores[i].mutex);
            }
        }
    }
    log_msg("Покупатель %d закончил покупки\n", id);
    return NULL;
}

void* loader_func(void* arg)
{
    while (!buyers_done)
    {
        for (int i = 0; i < NUM_STORES; ++i)
        {
            if (pthread_mutex_trylock(&stores[i].mutex) == 0)
            {
                if (!stores[i].occupied)
                {
                    stores[i].products += LOADER_ADD;
                    log_msg("Загрузчик добавил %d в магазин %d, сейчас там %d\n", LOADER_ADD, i, stores[i].products);
                }
                pthread_mutex_unlock(&stores[i].mutex);
            }
            sleep(1);
        }
    }
    log_msg("Загрузчик закончил работу\n");
    return NULL;
}

int main()
{
    srand(time(NULL));
    log_file = fopen("life.log", "w");
    if (!log_file)
    {
        perror("log file open");
        return 1;
    }

    for (int i = 0; i < NUM_STORES; ++i)
    {
        stores[i].products = rand() % STORE_MAX_INIT;
        pthread_mutex_init(&stores[i].mutex, NULL);
        stores[i].occupied = false;
    }
    pthread_t buyers[NUM_BUYERS], loader;
    int ids[NUM_BUYERS];
    for (int i = 0; i < NUM_BUYERS; ++i)
    {
        ids[i] = i + 1;
        pthread_create(&buyers[i], NULL, buyer_func, &ids[i]);
    }
    pthread_create(&loader, NULL, loader_func, NULL);
    for (int i = 0; i < NUM_BUYERS; ++i)
        pthread_join(buyers[i], NULL);

    buyers_done = true;
    pthread_join(loader, NULL);

    for (int i = 0; i < NUM_STORES; ++i)
        pthread_mutex_destroy(&stores[i].mutex);
    fclose(log_file);
    pthread_mutex_destroy(&log_mutex);

    printf("Симуляция жизни закончена. Смотрите life.log\n");
    return 0;
}
