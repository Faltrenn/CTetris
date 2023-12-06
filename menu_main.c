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

    while(1) {
        int selected = selection(stdscr, options, COLS, LINES);

        if(strcmp(options[selected], "Entrar") == 0) {
            menu_login();
        } else if(strcmp(options[selected], "Registrar-se") == 0) {
            menu_register();
        } else if(strcmp(options[selected], "Sair") == 0) {
            break;
        }
    }
    endwin();
}
