#include <stdio.h>

int input_choice(int max_choice)
{
    int choice;

    while (1)
    {
        printf("Выберите операцию (1-%d): ", max_choice);
        int ret = scanf("%d", &choice);
        if (ret != 1)
        {
            while (getchar() != '\n');
            printf("Ошибка ввода. Пожалуйста, введите число.\n");
            continue;
        }

        if (choice == max_choice)
        {
            printf("Выход...\n");
            return choice;
        }

        if ((choice < 1) || (choice > max_choice))
        {
            printf("Неверный выбор. Попробуйте снова.\n");
            continue;
        }

        return choice;
    }
}
