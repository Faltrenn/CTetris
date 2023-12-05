//
//  menu_home.c
//  CTetris
//
//  Created by Emanuel on 08/11/23.
//

#include "menu_home.h"
#include <ncurses.h>
#include <string.h>
#include "util.h"
#include "play.h"
#include "menu_score.h"

void menu_home(Player *player) {
    nodelay(stdscr, 0);

    keypad(stdscr, 1);

    start_color();
    init_pair(1, COLOR_BLUE, COLOR_BLACK);

    char *options[] = {
        "Jogar",
        "Records",
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
            if(strcmp(options[selected], "Jogar") == 0) {
                clear();
                char msg[COLS];
                sprintf(msg, "Player: %s", player->name);
                print_centered(stdscr, 0, COLS, LINES, COLS, msg, 0, 0);
                refresh();
                while(getch() != '\n');
                play(player);
            } else if(strcmp(options[selected], "Records") == 0) {
                menu_score(player);
            } else if(strcmp(options[selected], "Sair") == 0) {
                break;
            }
        }
        refresh();
    }
    endwin();
}
