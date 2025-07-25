#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "abonent.h"

struct abonent *head = NULL;
struct abonent *tail = NULL;

void add_abonent()
{
    struct abonent *new_node = malloc(sizeof(struct abonent));
    if (!new_node)
    {
        printf("Ошибка выделения памяти!\n");
        return;
    }
    printf("Введите имя: ");
    fgets(new_node->name, sizeof(new_node->name), stdin);
    new_node->name[strcspn(new_node->name, "\n")] = 0;

    printf("Введите фамилию: ");
    fgets(new_node->second_name, sizeof(new_node->second_name), stdin);
    new_node->second_name[strcspn(new_node->second_name, "\n")] = 0;

    printf("Введите телефон: ");
    fgets(new_node->tel, sizeof(new_node->tel), stdin);
    new_node->tel[strcspn(new_node->tel, "\n")] = 0;

    new_node->prev = tail;
    new_node->next = NULL;

    if (head == NULL)
    {
        head = tail = new_node;
    } else
    {
        tail->next = new_node;
        tail = new_node;
    }

    printf("Абонент добавлен.\n");
}

void delete_abonent()
{
    if (head == NULL)
    {
        printf("Справочник пуст.\n");
        return;
    }
    char del_name[10], del_second_name[10];
    printf("Введите имя для удаления: ");
    fgets(del_name, sizeof(del_name), stdin);
    del_name[strcspn(del_name, "\n")] = 0;

    printf("Введите фамилию для удаления: ");
    fgets(del_second_name, sizeof(del_second_name), stdin);
    del_second_name[strcspn(del_second_name, "\n")] = 0;

    struct abonent *current = head;
    int found = 0;
    while (current)
    {
        if (strcmp(current->name, del_name) == 0 && strcmp(current->second_name, del_second_name) == 0)
        {
            found = 1;

            if (current->prev)
                current->prev->next = current->next;
            else
                head = current->next;

            if (current->next)
                current->next->prev = current->prev;
            else
                tail = current->prev;

            free(current);
            printf("Абонент удалён.\n");
            break;
        }
        current = current->next;
    }
    if (!found)
        printf("Абонент с таким именем и фамилией не найден.\n");
}

void search_abonents()
{
    if (head == NULL)
    {
        printf("Справочник пуст.\n");
        return;
    }
    char search_name[10];
    printf("Введите имя для поиска: ");
    fgets(search_name, sizeof(search_name), stdin);
    search_name[strcspn(search_name, "\n")] = 0;

    struct abonent *current = head;
    int count = 0;
    while (current)
    {
        if (strcmp(current->name, search_name) == 0)
        {
            count++;
            printf("%d) %s %s, тел: %s\n", count, current->name, current->second_name, current->tel);
        }
        current = current->next;
    }
    if (count == 0)
    {
        printf("Абоненты с именем %s не найдены.\n", search_name);
    }
}

void print_all()
{
    if (head == NULL)
    {
        printf("Справочник пуст.\n");
        return;
    }
    struct abonent *current = head;
    int count = 0;
    while (current)
    {
        count++;
        printf("%d) %s %s, тел: %s\n", count, current->name, current->second_name, current->tel);
        current = current->next;
    }
}

void free_list()
{
    struct abonent *current = head;
    while (current)
    {
        struct abonent *next = current->next;
        free(current);
        current = next;
    }
    head = tail = NULL;
}
