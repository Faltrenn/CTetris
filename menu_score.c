//
//  menu_score.c
//  CTetris
//
//  Created by Emanuel on 14/11/23.
//

#include "play.h"
#include "menu_score.h"
#include <stdlib.h>
#include <string.h>

RecordList * create_recordlist(Record record, RecordList *next) {
    RecordList *nl = malloc(sizeof(RecordList));
    nl->record = record;
    nl->next = next;
    return nl;
}

RecordList * read_scores(char *file, FilterConfig fc) {
    RecordList *list = NULL;
    Record record;
    FILE *f = fopen("records.dat", "rb");
    for (fread(&record, sizeof(Record), 1, f); !feof(f); fread(&record, sizeof(Record), 1, f)) {
        if(fc.filter != CLEAR && ((fc.filter == BIGGERTHEN && record.score <= fc.f_value) || (fc.filter == SMALLERTHEN && record.score >= fc.f_value))) {
            continue;
        }
        if (list == NULL) {
            list = create_recordlist(record, list);
        } else {
            RecordList *last = NULL;
            for (RecordList *element = list; element != NULL; element = element->next) {
                if((fc.score == CRESCENT && record.score > element->record.score) || (fc.score == DECRESCENT && record.score < element->record.score)) {
                    if(last == NULL) {
                        list = create_recordlist(record, element);
                    } else {
                        last->next = create_recordlist(record, element);
                    }
                    break;
                } else if(element->next == NULL) {
                    element->next = create_recordlist(record, NULL);
                    break;
                }
                
                last = element;
            }
        }
    }
    fclose(f);
    return list;
}

void show_scores(WINDOW *w, RecordList *r_list) {
    wclear(w);
    int i = 0;
    for(RecordList *rl = r_list; rl != NULL; rl = rl->next) {
        char msg[COLS];
        sprintf(msg, "%s: %d", rl->record.name, rl->record.score);
        print_centered(w, i, COLS/2, LINES, COLS/2, msg, -1, -1);
        i++;
    }
    wrefresh(w);
    
}

int read_number(char *title) {
    int value_count = 0;
    char value[5] = {'\0', '\0', '\0', '\0', '\0'};
    
    while(1) {
        clear();
        
        print_centered(stdscr, 0, COLS, LINES, COLS, "Pressione ESC para voltar", 0, 0);
        
        print_centered(stdscr, 1, COLS, LINES, COLS, title, 0, 0);
        print_centered(stdscr, 2, COLS, LINES, COLS, "____", 0, 0);
        print_centered(stdscr, 2, COLS, LINES, 4, value, -1, 0);
        
        refresh();
        
        int key = getch();
        if(key == '\n') {
            return strcmp(value, "") != 0 ? atoi(value) : -1;
        } else if(key == 27) {
            for(int i = 0; i < 5; i++) {
                value[i] = '\0';
            }
            value_count = 0;
            break;
        } else if(key == 127 && value_count > 0) {
            value_count--;
            value[value_count] = '\0';
        } else if(value_count < 4 && key_is_digit(key)) {
            value[value_count] = key;
            value_count++;
        }
    }
    return -1;
}

void menu_score(Player *player) {
    WINDOW *w_l = newwin(LINES, COLS/2, 0, 0);
    WINDOW *w_r = newwin(LINES, COLS/2, 0, COLS/2);
    
    FilterConfig fc;
    fc.name = CRESCENT;
    fc.score = CRESCENT;
    fc.filter = CLEAR;
    fc.f_value = 0;
    
    start_color();
    
    show_scores(w_r, read_scores("records.dat", fc));

    while(1) {
        wclear(w_l);
        
        print_centered(w_l, 0, COLS/2, LINES, 0, "Pressione ESC para voltar", 0, -1);
        print_centered(w_l, 1, COLS/2, LINES, 0, "Digite f para filtrar", 0, -1);
        print_centered(w_l, 2, COLS/2, LINES, 0, "Digite o para organizar", 0, -1);
        
        wrefresh(w_l);
        
        int key = getch();
        if(key == 'o') {
            char *options[] = {
                "Crescente",
                "Decrescente",
                "Voltar",
                NULL
            };
            
            while(1) {
                show_scores(w_r, read_scores("records.dat", fc));
                
                int selected = selection(w_l, options, COLS/2, LINES);
                
                if(strcmp(options[selected], "Crescente") == 0) {
                    fc.score = CRESCENT;
                } else if(strcmp(options[selected], "Decrescente") == 0) {
                    fc.score = DECRESCENT;
                } else if(strcmp(options[selected], "Voltar") == 0) {
                    break;
                }
            }
        } else if(key == 'f') {
            char *options[] = {
                "Maior que",
                "Menor que",
                "Clear",
                "Voltar",
                NULL
            };
            
            while(1) {
                show_scores(w_r, read_scores("records.dat", fc));

                int selected = selection(w_l, options, COLS/2, LINES);

                if(strcmp(options[selected], "Maior que") == 0) {
                    int number = read_number("Digite o valor");
                    if(number != -1) {
                        fc.filter = BIGGERTHEN;
                        fc.f_value = number;
                    }
                } else if(strcmp(options[selected], "Menor que") == 0) {
                    int number = read_number("Digite o valor");
                    if(number != -1) {
                        fc.filter = SMALLERTHEN;
                        fc.f_value = number;
                    }
                } else if(strcmp(options[selected], "Clear") == 0) {
                    fc.filter = CLEAR;
                    fc.f_value = 0;
                } else if(strcmp(options[selected], "Voltar") == 0) {
                    break;
                }
            }
        } else if(key == 27) {
            break;
        }
    }
}
