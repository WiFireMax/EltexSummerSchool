#include <stdio.h>

int main()
{
    unsigned int num;
    unsigned char new_byte;
    printf("Введите положительное целое число: ");
    scanf("%u", &num);

    printf("Введите новое значение третьего байта (0-255): ");
    scanf("%hhu", &new_byte);

    num = (num & 0xFF00FFFF) | (new_byte << 16);

    printf("Результат: %u\n", num);
    return 0;
}
