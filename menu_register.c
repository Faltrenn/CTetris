//
//  menu_register.c
//  CTetris
//
//  Created by Emanuel on 08/11/23.
//

#include "menu_register.h"
#include <ncurses.h>
#include "util.h"


void menu_register(void) {
    nodelay(stdscr, 0);
    curs_set(0);

    keypad(stdscr, 1);

    int selected = 0, name_count = 0, password_count = 0;
    char name[7] = {'\0', '\0', '\0', '\0', '\0', '\0', '\0'};
    char password[10] = {'\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0'};
    
    while(1) {
        clear();
        print_centered(0, 15, "Digite seu nome", 0);
        print_centered(1, 6, "______", 0);
        print_centered(1, 6, name, -1);
        
        if(selected > 0) {
            print_centered(2, 16, "Digite sua senha", 0);
            print_centered(3, 9, "_________", 0);
            print_centered(3, 9, password, -1);
        }

        int key = getch();
        if(key == '\n') {
            selected++;
            if(selected == 2) {
                // Validar registro
                if(try_register(name, password)) {
                    
                    break;
                } else {
                    clear();
                    print_centered(0, COLS, "Nenhum usuário com essa senha foi encontrado.", 0);
                    print_centered(1, COLS, "Pressione ENTER para sair", 0);
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
