//
//  menu_login.c
//  CTetris
//
//  Created by Emanuel on 07/11/23.
//

#include "menu_login.h"
#include <ncurses.h>
#include <string.h>
#include <stdlib.h>
#include "util.h"
#include "menu_main.h"
#include "menu_home.h"

void menu_login(void) {
    nodelay(stdscr, 0);

    keypad(stdscr, 1);

    int selected = 0, name_count = 0, password_count = 0;
    char name[7] = {'\0', '\0', '\0', '\0', '\0', '\0', '\0'};
    char password[10] = {'\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0'};
    
    while(1) {
        clear();

        print_centered(stdscr, 0, COLS, LINES, COLS, "Pressione ESC para voltar", 0, 0);
        
        print_centered(stdscr, 1, COLS, LINES, COLS, "Digite seu nome", 0, 0);
        print_centered(stdscr, 2, COLS, LINES, COLS, "______", 0, 0);
        print_centered(stdscr, 2, COLS, LINES, 6, name, -1, 0);
        
        if(selected > 0) {
            print_centered(stdscr, 3, COLS, LINES, COLS, "Digite sua senha", 0, 0);
            print_centered(stdscr, 4, COLS, LINES, COLS, "_________", 0, 0);
            print_centered(stdscr, 4, COLS, LINES, 9, password, -1, 0);
        }

        int key = getch();
        if(key == '\n') {
            selected++;
            if(selected == 2) {
                // Validar login
                Player *player = malloc(sizeof(Player));
                int code = try_login(name, password, player);
                if(code == 1) {
                    menu_home(player);
                    break;
                }else {
                    clear();
                    if(code == 5)
                        print_centered(stdscr, 0, COLS, LINES, COLS, "Usuario ou senha invalidos.", 0, 0);
                    else if(code == 0)
                        print_centered(stdscr, 0, COLS, LINES, COLS, "Nenhum usuario com essa senha foi encontrado.", 0, 0);
                    
                    print_centered(stdscr, 1, COLS, LINES, COLS, "Pressione ENTER para sair", 0, 0);
                    refresh();
                    
                    while(getch() != '\n');
                    
                    for(int i = 0; i < 7; i++) {
                        name[i] = '\0';
                    }
                    name_count = 0;
                    for (int i = 0; i < 10; i++) {
                        password[i] = '\0';
                    }
                    password_count = 0;
                    
                    selected = 0;
                }
            }
        } else if(key == 27) {
            for(int i = 0; i < 7; i++) {
                name[i] = '\0';
            }
            name_count = 0;
            for (int i = 0; i < 10; i++) {
                password[i] = '\0';
            }
            password_count = 0;
            
            selected = 0;
            break;
        } else if(selected == 0) {
            if(key == 127 && name_count > 0) {
                name_count--;
                name[name_count] = '\0';
            } else if(name_count < 6 && key_is_letter(key)) {
                name[name_count] = key;
                name_count++;
            }
        } else if(selected == 1) {
            if(key == 127 && password_count > 0) {
                password_count--;
                password[password_count] = '\0';
            } else if (password_count < 9 && key_is_letter(key)) {
                password[password_count] = key;
                password_count++;
            }
        }
        refresh();
    }
}
