#include <stdio.h>
#include <stdlib.h>
#include "abonent.h"

int main()
{
    while (1)
    {
        printf("\n////АБОНЕНТСКИЙ СПРАВОЧНИК////\n");
        printf("1) Добавить абонента\n");
        printf("2) Удалить абонента\n");
        printf("3) Поиск абонентов по имени\n");
        printf("4) Вывод всех записей\n");
        printf("5) Выход\n");
        printf("Введите пункт меню: ");

        int choice;
        if (scanf("%d", &choice) != 1)
        {
            printf("Неверный ввод!\n");
            int c; while ((c = getchar()) != '\n' && c != EOF);
            continue;
        }
        int c; while (getchar() != '\n');

        switch (choice)
        {
            case 1: add_abonent(); break;
            case 2: delete_abonent(); break;
            case 3: search_abonents(); break;
            case 4: print_all(); break;
            case 5:
                free_list();
                printf("Выход.\n");
                return 0;
            default:
                printf("Некорректный пункт меню.\n");
        }
    }
}
