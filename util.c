//
//  utils.c
//  CTetris
//
//  Created by Emanuel on 08/11/23.
//

#include "util.h"
#include <ncurses.h>
#include <string.h>
#include <stdlib.h>

void print_centered(WINDOW *w, int line_offset, int width, int height, int n, char *msg, int horizontal, int vertical) {
    int col_pos = width/2 - (int)strlen(msg)/2;
    if (horizontal == -1) {
        col_pos = width/2 - (n/2);
    } else if(horizontal == 1) {
        col_pos = width/2 + (n/2);
    }
    
    int line_pos = height/2-1+line_offset;
    if (vertical == -1) {
        line_pos = line_offset;
    } else if(vertical == 1) {
        line_pos = height;
    }
    mvwprintw(w, line_pos, col_pos, "%s", msg);
}

int key_is_letter(char key) {
    return (key >=65 && key <= 90) || (key >= 97 && key <= 122);
}

int verify_name_and_password(char name[7], char password[10]) {
    if(strlen(name) == 0 || strlen(password) == 0)
        return 5; // Nome ou senha vazios
    return 1;
}

int try_login(char name[7], char password[10], Player *player) {
    int code = verify_name_and_password(name, password);
    if(code != 1)
        return code;
    FILE *file = fopen("players.dat", "rb");
    if(file != NULL){
        Player *p = malloc(sizeof(Player));
        while(!feof(file)) {
            fread(p, sizeof(Player), 1, file);
            if(strcmp(p->name, name) == 0) {
                *player = *p;
                free(p);
                fclose(file);
                return 1;
            }
        }
    }
    fclose(file);
    return 0;
}

int try_register(char name[7], char password[10]) {
    int code = verify_name_and_password(name, password);
    if(code != 1)
        return code;
    if(search_player(name) != NULL)
        return 6;// Usuário existe
    
    Player *player = malloc(sizeof(Player));
    strcpy(player->name, name);
    strcpy(player->password, password);
    
    FILE *file = fopen("players.dat", "ab");
    if(file == NULL) {
        file = fopen("players", "wb");
    }
    fwrite(player, sizeof(player), 1, file);
    
    fclose(file);
    return 1;
}

Player * search_player(char name[7]) {
    FILE *file = fopen("players.dat", "rb");
    if(file != NULL) {
        Player *player = malloc(sizeof(Player));
        while(!feof(file)) {
            fread(player, sizeof(Player), 1, file);
            return player;
        }
        fclose(file);
    }
    return NULL;
}

int select_option(WINDOW *w, char *options[], char *title, unsigned int show_start) {
    start_color();
    init_pair(1, COLOR_BLUE, COLOR_BLACK);
    
    unsigned int selected = 0;
    int key = 0;
    while(key != 27) {
        int start = show_start;
        wclear(w);
        print_centered(w, start++, COLS/2, LINES, COLS/2, "Pressione ESC para voltar", 0, -1);
        print_centered(w, start++, COLS/2, LINES, COLS/2, title, 0, -1);
        int i = 0;
        for(char *option = options[0]; option != NULL; option = options[++i]) {
            if(i == selected) {
                attron(COLOR_PAIR(1));
                print_centered(w, start + i, COLS/2, LINES, COLS/2, option, 0, -1);
                attroff(COLOR_PAIR(1));
            } else {
                print_centered(w, start + i, COLS/2, LINES, COLS/2, option, 0, -1);
            }
        }
        wrefresh(w);
        
        key = getch();
        while(key != 27) {
            if(key == '\n' || key == KEY_DOWN || key == KEY_UP) {
                return selected;
            } else if(key == KEY_DOWN) {
                if(++selected >= i)
                    selected--;
            } else if(key == KEY_UP) {
                if(--selected < 0)
                    selected++;
            }
            break;
        }
    }
    return -1;
}


