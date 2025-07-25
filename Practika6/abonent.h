#ifndef _ABONENT_H_
#define _ABONENT_H_

struct abonent
{
    char name[10];
    char second_name[10];
    char tel[10];
    struct abonent *prev;
    struct abonent *next;
};

extern struct abonent *head;
extern struct abonent *tail;

void add_abonent(void);
void delete_abonent(void);
void search_abonents(void);
void print_all(void);
void free_list(void);

#endif
