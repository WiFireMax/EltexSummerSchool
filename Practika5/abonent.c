#include <stdio.h>
#include <string.h>
#include "abonent.h"

void add_abonent(struct abonent *book, int *count)
{
    if (*count >= MAX_ABONENTS)
    {
        printf("Справочник переполнен!\n");
        return;
    }
    int idx = -1;
    for (int i = 0; i < MAX_ABONENTS; i++)
    {
        if (book[i].name[0] == '\0')
        {
            idx = i;
            break;
        }
    }
    if (idx == -1)
    {
        printf("Справочник переполнен!\n");
        return;
    }
    struct abonent a;
    printf("Введите имя: ");
    fgets(a.name, sizeof(a.name), stdin);
    a.name[strcspn(a.name, "\n")] = 0;

    printf("Введите фамилию: ");
    fgets(a.second_name, sizeof(a.second_name), stdin);
    a.second_name[strcspn(a.second_name, "\n")] = 0;

    printf("Введите телефон: ");
    fgets(a.tel, sizeof(a.tel), stdin);
    a.tel[strcspn(a.tel, "\n")] = 0;

    book[idx] = a;
    (*count)++;
    printf("Добавлено под номером %d\n", idx + 1);
}

void delete_abonent(struct abonent *book, int *count) {
    printf("Введите номер абонента для удаления (от 1 до %d): ", MAX_ABONENTS);
    int idx;
    scanf("%d", &idx);
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
    idx--;
    if (idx >= 0 && idx < MAX_ABONENTS && book[idx].name[0] != '\0')
    {
        memset(&book[idx], 0, sizeof(struct abonent));
        (*count)--;
        printf("Абонент удален.\n");
    } else
    {
        printf("Нет такого абонента!\n");
    }
}

void search_abonent(const struct abonent *book) {
    char search_name[10];
    printf("Введите имя для поиска: ");
    fgets(search_name, sizeof(search_name), stdin);
    search_name[strcspn(search_name, "\n")] = 0;
    int found = 0;
    for (int i = 0; i < MAX_ABONENTS; i++)
    {
        if (book[i].name[0] != '\0' && strcmp(book[i].name, search_name) == 0)
        {
            printf("%d) %s %s, %s\n", i + 1, book[i].name, book[i].second_name, book[i].tel);
            found = 1;
        }
    }
    if (!found)
        printf("Абоненты не найдены.\n");
}

void print_all(const struct abonent *book)
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
    if (!any)
        printf("Справочник пуст.\n");
}
