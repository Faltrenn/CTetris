#include <ncurses.h>
#include <string.h>
#include "menu_play.h"
#include "menu_main.h"

void menu_main() {
    initscr();
    cbreak();
    noecho();
    nodelay(stdscr, 0);
    curs_set(0);

    keypad(stdscr, TRUE);

    start_color();
    init_pair(1, COLOR_BLUE, COLOR_BLACK);

    char *options[] = {
        "Jogar",
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
                mvprintw(LINES/2-2+i, COLS/2 - strlen(options[i])/2, "%s", options[i]);
                attroff(COLOR_PAIR(1));
            } else {
                mvprintw(LINES/2-2+i, COLS/2 - strlen(options[i])/2, "%s", options[i]);
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
            if(strcmp(options[selected], "Jogar") == 0) {
                menu_play();
            } else if(strcmp(options[selected], "Sair") == 0) {
                break;
            }
        }

        refresh();
    }

    endwin();
}
