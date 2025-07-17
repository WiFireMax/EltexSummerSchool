#include <stdio.h>

int main()
{
    unsigned int n;
    int count = 0;
    printf("Введите положительное целое число: ");
    scanf("%u", &n);

    for (int i = 0; i < sizeof(n)*8; i++)
    {
        if ((n >> i) & 1) count++;
    }
    printf("Количество единиц: %d\n", count);
    return 0;
}
