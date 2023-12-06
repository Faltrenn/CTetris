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
        int selected = selection(stdscr, options, COLS, LINES);

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
    endwin();
}
