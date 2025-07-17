#include <stdio.h>

int main()
{
    int n;
    printf("Введите отрицательное целое число: ");
    scanf("%d", &n);

    printf("Двоичное представление: ");
    for (int i = sizeof(n)*8 - 1; i >= 0; i--)
    {
        printf("%d", (n >> i) & 1);
    }
    printf("\n");
    return 0;
}
