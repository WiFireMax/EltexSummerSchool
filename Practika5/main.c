#include <stdio.h>
#include "abonent.h"
#include "menu.h"

int main()
{
    struct abonent book[MAX_ABONENTS] = {0};
    int abonent_count = 0;

    while (1)
    {
        print_menu();
        int choice;
        scanf("%d", &choice);
        int c;
        while (getchar() != '\n');

        switch (choice)
        {
            case 1: add_abonent(book, &abonent_count); break;
            case 2: delete_abonent(book, &abonent_count); break;
            case 3: search_abonent(book); break;
            case 4: print_all(book); break;
            case 5:
                printf("Выход.\n");
                return 0;
            default:
                printf("Некорректный пункт меню!\n");
        }
    }
}
