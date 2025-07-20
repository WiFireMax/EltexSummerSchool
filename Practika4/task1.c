#include <stdio.h>
#include <string.h>

#define MAX_ABONENTS 100

struct abonent
{
    char name[10];
    char second_name[10];
    char tel[10];
};

int main()
{
    struct abonent book[MAX_ABONENTS] = {0};
    int abonent_count = 0;

    while (1) {
        printf("\n////////АБОНЕНТСКИЙ СПРАВОЧНИК///////\n");
        printf("1) Добавить абонента\n");
        printf("2) Удалить абонента\n");
        printf("3) Поиск абонентов по имени\n");
        printf("4) Вывод всех записей\n");
        printf("5) Выход\n");
        printf("Выберите пункт: ");

        int choice;
        scanf("%d", &choice);
        while (getchar() != '\n');


        switch (choice)
        {
            case 1:
            {
                if (abonent_count >= MAX_ABONENTS)
                {
                    printf("Справочник переполнен!\n");
                    break;
                }
                int idx = -1;
                for (int i = 0; i < MAX_ABONENTS; i++)
                {
                    if (book[i].name[0] == '\0') { idx = i; break; }
                }
                if (idx == -1)
                {
                    printf("Справочник переполнен!\n");
                    break;
                }
                struct abonent a;
                printf("Введите имя: ");
                fgets(a.name, sizeof(a.name), stdin); a.name[strcspn(a.name, "\n")] = 0;
                printf("Введите фамилию: ");
                fgets(a.second_name, sizeof(a.second_name), stdin); a.second_name[strcspn(a.second_name, "\n")] = 0;
                printf("Введите телефон: ");
                fgets(a.tel, sizeof(a.tel), stdin); a.tel[strcspn(a.tel, "\n")] = 0;
                book[idx] = a;
                abonent_count++;
                printf("Добавлено под номером %d\n", idx + 1);
                break;
            }
            case 2:
            {
                printf("Введите номер абонента для удаления (от 1 до %d): ", MAX_ABONENTS);
                int idx;
                scanf("%d", &idx); getchar();
                idx--;
                if (idx >= 0 && idx < MAX_ABONENTS && book[idx].name[0] != '\0')
                {
                    memset(&book[idx], 0, sizeof(struct abonent));
                    abonent_count--;
                    printf("Абонент удален.\n");
                } else
                {
                    printf("Нет такого абонента!\n");
                }
                break;
            }
            case 3: {
                char search_name[10];
                printf("Введите имя для поиска: ");
                fgets(search_name, sizeof(search_name), stdin); search_name[strcspn(search_name, "\n")] = 0;
                int found = 0;
                for (int i = 0; i < MAX_ABONENTS; i++)
                {
                    if (book[i].name[0] != '\0' && strcmp(book[i].name, search_name) == 0)
                    {
                        printf("%d) %s %s, %s\n", i + 1, book[i].name, book[i].second_name, book[i].tel);
                        found = 1;
                    }
                }
                if (!found) printf("Абоненты не найдены.\n");
                break;
            }
            case 4:
            {
                int any = 0;
                for (int i = 0; i < MAX_ABONENTS; i++)
                {
                    if (book[i].name[0] != '\0')
                    {
                        printf("%d) %s %s, %s\n", i + 1, book[i].name, book[i].second_name, book[i].tel);
                        any = 1;
                    }
                }
                if (!any) printf("Справочник пуст.\n");
                break;
            }
            case 5:
                printf("Выход.\n");
                return 0;
            default:
                printf("Некорректный пункт меню!\n");
        }
    }

    return 0;
}
