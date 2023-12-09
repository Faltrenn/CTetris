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

int key_is_digit(char key) {
    return (key >= '0' && key <= '9');
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
    FILE *file = fopen("players.dat", "r+b");
    if(file != NULL){
        Player *p = malloc(sizeof(Player));
        while(!feof(file)) {
            fread(p, sizeof(Player), 1, file);
            if(strcmp(p->name, name) == 0) {
                if(strcmp(p->password, password) != 0 ) {
                    return 0;
                }
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
    
    FILE *file = fopen("players.dat", "a+b");
    if(file == NULL) {
        file = fopen("players", "w+b");
    }
    fwrite(player, sizeof(Player), 1, file);
    
    fclose(file);
    return 1;
}

Player * search_player(char name[7]) {
    FILE *file = fopen("players.dat", "r+b");
    if(file != NULL) {
        Player *player = malloc(sizeof(Player));
        while(!feof(file)) {
            fread(player, sizeof(Player), 1, file);
            if(strcmp(player->name, name) == 0) {
                fclose(file);
                return player;
            }
        }
        fclose(file);
    }
    return NULL;
}

int selection(WINDOW *w, char *options[], unsigned int width, unsigned int height) {
    start_color();
    init_pair(1, COLOR_BLUE, COLOR_BLACK);
    
    int selected = 0;
    while(1) {
        wclear(w);
        int i = 0;
        for(i = 0; options[i] != NULL; i++) {
            if(i == selected) {
                wattron(w, COLOR_PAIR(1));
                print_centered(w, i, width, height, width/2, options[i], 0, 0);
                wattroff(w, COLOR_PAIR(1));
            } else {
                print_centered(w, i, width, height, width/2, options[i], 0, 0);
            }
        }
        wrefresh(w);
        
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
            return selected;
        }
    }
}

