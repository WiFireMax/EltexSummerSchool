#include <stdio.h>
#include <string.h>

#define MAX 100

char* pos_in_str(char* str, char* substr)
{
    for (; *str; str++)
    {
        char *s = str;
        char *t = substr;
        while (*t && *s == *t)
        {
            s++;
            t++;
        }
        if (!*t)
            return str;
    }
    return NULL;
}

int main()
{
    char str[MAX], substr[MAX];
    printf("Введите строку: ");
    fgets(str, MAX, stdin);
    str[strcspn(str, "\n")] = '\0';

    printf("Введите подстроку: ");
    fgets(substr, MAX, stdin);
    substr[strcspn(substr, "\n")] = '\0';

    char *pos = pos_in_str(str, substr);
    if (pos)
        printf("Подстрока найдена по адресу: %p (%s)\n", (void*)pos, pos);
    else
        printf("Подстрока не найдена.\n");
    return 0;
}
