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
                // Validar registro
                int code = try_register(name, password);
                if(code == 1) {
                    break;
                } else {
                    clear();
                    if(code == 6)
                        print_centered(stdscr, 0, COLS, LINES, COLS, "Usuario ja existente.", 0, 0);
                    else if(code == 5)
                        print_centered(stdscr, 0, COLS, LINES, COLS, "Usuario ou senha invalidos.", 0, 0);
                    
                    print_centered(stdscr, 1, COLS, LINES, COLS, "Pressione ENTER para sair", 0, 0);
                    refresh();
                    
                    while(getch() != '\n');

                    for(int i = 0; i < 7; i++)
                        name[i] = '\0';
                    name_count = 0;
                    
                    for (int i = 0; i < 10; i++)
                        password[i] = '\0';
                    password_count = 0;
                    
                    selected = 0;
                }
            }
        }else if(key == 27) {
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
