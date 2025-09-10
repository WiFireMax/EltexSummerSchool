#ifndef TIME_ASK
#define TIME_ASK

#include <time.h>
#include <string.h>
#include <stdio.h>

static void get_current_time_str(char *buffer, size_t size)
{
    time_t now = time(NULL);
    struct tm *tm_info = localtime(&now);
    strftime(buffer, size, "%Y-%m-%d %H:%M:%S", tm_info);
}

#endif
