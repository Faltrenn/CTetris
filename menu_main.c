#include <ncurses.h>
#include <string.h>
#include "menu_main.h"
#include "menu_login.h"
#include "menu_register.h"
#include "util.h"

void menu_main(void) {
    initscr();
    cbreak();
    noecho();
    nodelay(stdscr, 0);
    curs_set(0);

    keypad(stdscr, 1);

    start_color();
    init_pair(1, COLOR_BLUE, COLOR_BLACK);

    char *options[] = {
        "Entrar",
        "Registrar-se",
        "Sair",
        NULL
    };

    int selected = 0;
    while(1) {
        clear();
        int i = 0;
        for(i = 0; options[i] != NULL; i++) {
            if(i == selected) {
                attron(COLOR_PAIR(1));
                print_centered(stdscr, i, COLS, LINES, COLS, options[i], 0, 0);
                attroff(COLOR_PAIR(1));
            } else {
                print_centered(stdscr, i, COLS, LINES, COLS, options[i], 0, 0);
            }
        }

        int key = getch();
        if(key == KEY_DOWN) {
            if(selected < i-1) {
                selected++;
            }
        } else if(key == KEY_UP) {
            if(selected > 0) {
                selected--;
            }
        } else if(key == '\n') {
            if(strcmp(options[selected], "Entrar") == 0) {
                menu_login();
            } else if(strcmp(options[selected], "Registrar-se") == 0) {
                menu_register();
            } else if(strcmp(options[selected], "Sair") == 0) {
                break;
            }
        }
        refresh();
    }
    endwin();
}
