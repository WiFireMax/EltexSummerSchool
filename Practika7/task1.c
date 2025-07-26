#include <stdio.h>
#include <calculator.h>

int main() {
    int choice;
    int a, b;
    int result;

    while (1) {
        printf("Калькулятор\n");
        printf("1. Сложение\n");
        printf("2. Вычитание\n");
        printf("3. Умножение\n");
        printf("4. Деление\n");
        printf("5. Выход\n");

        choice = input_choice(5);

        if (choice == 5)
        {
            break;
        }

        printf("Введите два целых числа: ");
        scanf("%d %d", &a, &b);

        switch (choice)
        {
            case 1:
                result = add(a, b);
                printf("Результат: %d\n", result);
                break;
            case 2:
                result = sub(a, b);
                printf("Результат: %d\n", result);
                break;
            case 3:
                result = mul(a, b);
                printf("Результат: %d\n", result);
                break;
            case 4:
                if (b == 0)
                {
                    printf("Ошибка: Деление на ноль!\n");
                } else
                {
                    result = divide(a, b);
                    printf("Результат: %d\n", result);
                }
                break;
        }
    }

    return 0;
}
