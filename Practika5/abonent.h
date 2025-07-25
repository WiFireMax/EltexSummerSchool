#ifndef _ABONENT_H_
#define _ABONENT_H_

#define MAX_ABONENTS 100

struct abonent
{
    char name[10];
    char second_name[10];
    char tel[10];
};

void add_abonent(struct abonent *book, int *count);
void delete_abonent(struct abonent *book, int *count);
void search_abonent(const struct abonent *book);
void print_all(const struct abonent *book);

#endif
