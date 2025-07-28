#include <stdio.h>
#include <stdlib.h>

int main()
{
    const char *filename = "output.txt";
    const char *text = "String from file";

    FILE *f = fopen(filename, "w");
    if (!f)
    {
        perror("Ошибка при открытии файла для записи");
        return 1;
    }
    fprintf(f, "%s", text);
    fclose(f);

    f = fopen(filename, "rb");
    if (!f)
    {
        perror("Ошибка при открытии файла для чтения");
        return 1;
    }

    fseek(f, 0, SEEK_END);
    long filesize = ftell(f);

    for (long i = filesize - 1; i >= 0; i--)
    {
        fseek(f, i, SEEK_SET);
        int c = fgetc(f);
        if (c == EOF) break;
        putchar(c);
    }
    putchar('\n');

    fclose(f);
    return 0;
}
