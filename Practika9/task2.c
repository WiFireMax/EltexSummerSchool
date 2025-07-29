#include <ncurses.h>
#include <unistd.h>
#include <dirent.h>
#include <string.h>
#include <stdlib.h>

#define MAX_FILES 1024
#define CTRL_TAB 9

typedef struct
{
    WINDOW *win;
    char path[PATH_MAX];
    char *files[MAX_FILES];
    int file_count;
    int selected_index;
} Panel;

void init_ncurses()
{
    initscr();
    noecho();
    cbreak();
    keypad(stdscr, TRUE);
    curs_set(FALSE);
}

int filecmp(const void *a, const void *b) {
    const char *fa = *(const char **)a;
    const char *fb = *(const char **)b;

    if (strcmp(fa, "..") == 0) return -1;
    if (strcmp(fb, "..") == 0) return 1;

    if (strcmp(fa, ".") == 0) return -1;
    if (strcmp(fb, ".") == 0) return 1;

    return strcmp(fa, fb);
}

void load_dir(Panel *p)
{
    DIR *dir = opendir(p->path);
    struct dirent *entry;
    p->file_count = 0;

    while((entry = readdir(dir)) != NULL && p->file_count < MAX_FILES)
    {
        p->files[p->file_count] = strdup(entry->d_name);
        p->file_count++;
    }
    closedir(dir);
    qsort(p->files, p->file_count, sizeof(char *), filecmp);
}

void draw_panel(Panel *p, int highlight)
{
    werase(p->win);
    box(p->win, 0, 0);
    mvwprintw(p->win, 0, 2, " %s ", p->path);
    for(int i = 0; i < p->file_count; i++)
    {
        if(i == p->selected_index && highlight)
            wattron(p->win, A_REVERSE);
        mvwprintw(p->win, i+1, 1, "%s", p->files[i]);
        if(i == p->selected_index && highlight)
            wattroff(p->win, A_REVERSE);
    }
    wrefresh(p->win);
}

void free_panel_files(Panel *p)
{
    for(int i = 0; i < p->file_count; i++)
    {
        free(p->files[i]);
    }
}

int main()
{
    init_ncurses();

    int height = LINES - 2;
    int width = COLS/2 - 2;

    Panel panel1, panel2;
    panel1.win = newwin(height, width, 1, 1);
    panel2.win = newwin(height, width, 1, width + 3);

    getcwd(panel1.path, sizeof(panel1.path));
    strcpy(panel2.path, panel1.path);

    load_dir(&panel1);
    load_dir(&panel2);

    int active_panel = 0;
    panel1.selected_index = 0;
    panel2.selected_index = 0;

    int ch;
    while((ch = getch()) != 'q')
    {
        Panel *p = active_panel == 0 ? &panel1 : &panel2;

        switch(ch)
        {
            case KEY_UP:
                if(p->selected_index > 0) p->selected_index--;
                break;
            case KEY_DOWN:
                if(p->selected_index < p->file_count - 1) p->selected_index++;
                break;
            case '\t':
                active_panel = 1 - active_panel;
                break;
            case '\n':
            {
                char new_path[PATH_MAX];

                const char *selected = p->files[p->selected_index];

                if (strcmp(selected, "..") == 0)
                {
                    if (strcmp(p->path, "/") != 0)
                    {
                        char *last_slash = strrchr(p->path, '/');
                        if (last_slash != NULL)
                        {
                            if (last_slash == p->path)
                            {
                                *(last_slash + 1) = '\0';
                            } else {
                                *last_slash = '\0';
                            }
                        }
                    }
                    free_panel_files(p);
                    load_dir(p);
                    p->selected_index = 0;
                } else if (strcmp(selected, ".") == 0) {
                    free_panel_files(p);
                    load_dir(p);
                    p->selected_index = 0;
                } else {
                    snprintf(new_path, sizeof(new_path), "%s/%s", p->path, selected);
                    DIR *dir = opendir(new_path);
                    if (dir)
                    {
                        closedir(dir);
                        strcpy(p->path, new_path);
                        free_panel_files(p);
                        load_dir(p);
                        p->selected_index = 0;
                    }
                }
                break;
            }
        }

        draw_panel(&panel1, active_panel == 0);
        draw_panel(&panel2, active_panel == 1);
    }

    free_panel_files(&panel1);
    free_panel_files(&panel2);

    endwin();
    return 0;
}
