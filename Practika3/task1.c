#include <stdio.h>

int main()
{
    unsigned int num;
    unsigned char byte;
    printf("Введите положительное целое число: ");
    scanf("%u", &num);
    printf("Введите новое значение третьего байта (0-255): ");
    scanf("%hhu", &byte);

    unsigned char *ptr = (unsigned char*)&num;
    ptr[2] = byte;

    printf("Результат: %u\n", num);

    printf("Байты (от младшего к старшему): ");
    for (int i = 0; i < sizeof(num); i++)
        printf("%02X ", ptr[i]);
    printf("\n");
    return 0;
}
