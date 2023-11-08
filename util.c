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

void print_centered(int line_offset, int n, char *msg, int horizontal) {
    int line_pos = LINES/2-1+line_offset;
    int col_pos = COLS/2 - (int)strlen(msg)/2;
    if (horizontal == -1) {
        col_pos = COLS/2 - (n/2);
    } else if(horizontal == 1) {
        col_pos = COLS/2 + (n/2);
    }
    mvprintw(line_pos, col_pos, "%s", msg);
}

int key_is_letter(char key) {
    return (key >=65 && key <= 90) || (key >= 97 && key <= 122);
}

int verify_name_and_password(char name[7], char password[10]) {
    if(strlen(name) == 0 || strlen(password) == 0)
        return 5; // Nome ou senha vazios
    return 1;
}

int try_login(char name[7], char password[10]) {
    int code = verify_name_and_password(name, password);
    if(code != 1)
        return code;
    FILE *file = fopen("players.dat", "rb");
    if(file != NULL){
        Player player;
        while(!feof(file)) {
            fread(&player, sizeof(Player), 1, file);
            if(strcmp(player.name, name) == 0) {
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
    fwrite(player, sizeof(Player), 1, file);
    
    clear();
    print_centered(0, COLS, player->name,0);
    refresh();
    
    while (getch() != '\n');
    
    fclose(file);
    return 1;
}

Player * search_player(char name[7]) {
    FILE *file = fopen("players.dat", "rb");
    if(file != NULL) {
        Player player;
        while(!feof(file)) {
            fread(&player, sizeof(Player), 1, file);
        }
        fclose(file);
    }
    return NULL;
}
