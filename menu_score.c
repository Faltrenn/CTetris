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
#include "util.h"

RecordList * create_recordlist(Record record, RecordList *next) {
    RecordList *nl = malloc(sizeof(RecordList));
    nl->record = record;
    nl->next = next;
    return nl;
}

int verify_filter(FilterConfig fc, Record record) {
    return (fc.filter != CLEAR && (
                                   (fc.filter == BIGGERTHEN && record.score <= fc.f_value) ||
                                   (fc.filter == SMALLERTHEN && record.score >= fc.f_value))) ||
           (fc.view == PLAYER && strcmp(record.name, fc.player->name) != 0);
}

RecordList * read_scores(char *file, FilterConfig fc, RecordList *last) {
    free_recordlist(last);
    RecordList *list = NULL;
    Record record;
    FILE *f = fopen("records.dat", "r+b");
    if(f != NULL) {
        for (fread(&record, sizeof(Record), 1, f); !feof(f); fread(&record, sizeof(Record), 1, f)) {
            if(record.deleted == 0) {
                if(verify_filter(fc, record)) {
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
        }
        fclose(f);
    }
    return list;
}

RecordList * get_recordlist_element(RecordList *rl, unsigned int position) {
    int i = 0;
    for (RecordList *element = rl; element != NULL; element = element->next) {
        if(i == position)
            return element;
        i++;
    }
    return NULL;
}

void show_scores(WINDOW *w, RecordList *r_list, int selected) {
    start_color();
    init_pair(1, COLOR_BLUE, COLOR_BLACK);

    wclear(w);
    int i = 0;
    for(RecordList *rl = r_list; rl != NULL; rl = rl->next) {
        char msg[COLS];
        sprintf(msg, "%s: %d", rl->record.name, rl->record.score);
        if (selected == i) {
            wattron(w, COLOR_PAIR(1));
            print_centered(w, i, COLS/2, LINES, COLS/2, msg, -1, -1);
            wattroff(w, COLOR_PAIR(1));
        } else {
            print_centered(w, i, COLS/2, LINES, COLS/2, msg, -1, -1);
        }
        i++;
    }
    wrefresh(w);
}

void free_recordlist(RecordList *rl) {
    if(rl != NULL)
        if(rl->next != NULL)
            free_recordlist(rl->next);
    free(rl);
}

void menu_score(Player *player) {
    WINDOW *w_l = newwin(LINES, COLS/2, 0, 0);
    WINDOW *w_r = newwin(LINES, COLS/2, 0, COLS/2);
    
    FilterConfig fc;
    fc.name = CRESCENT;
    fc.score = CRESCENT;
    fc.filter = CLEAR;
    fc.f_value = 0;
    fc.view = ALL;
    fc.player = player;
    
    start_color();
    
    RecordList *rl = NULL;
    
    while(1) {
        rl = read_scores("records.dat", fc, rl);
        show_scores(w_r, rl, -1);
    
        wclear(w_l);
        
        print_centered(w_l, 0, COLS/2, LINES, 0, "Pressione ESC para voltar", 0, -1);
        print_centered(w_l, 1, COLS/2, LINES, 0, "Digite 'f' para filtrar", 0, -1);
        print_centered(w_l, 2, COLS/2, LINES, 0, "Digite 'o' para organizar", 0, -1);
        print_centered(w_l, 3, COLS/2, LINES, 0, "Digite 'b' para focar", 0, -1);
        print_centered(w_l, 4, COLS/2, LINES, 0, "Digite 'd' para deletar", 0, -1);
        
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
                rl = read_scores("records.dat", fc, rl);
                show_scores(w_r, rl, -1);
                
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
                "Remover",
                "Voltar",
                NULL
            };
            
            while(1) {
                rl = read_scores("records.dat", fc, rl);
                show_scores(w_r, rl, -1);
                
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
                } else if(strcmp(options[selected], "Remover") == 0) {
                    fc.filter = CLEAR;
                    fc.f_value = 0;
                } else if(strcmp(options[selected], "Voltar") == 0) {
                    break;
                }
            }
        } else if(key == 'b') {
            char *options[] = {
                "Todos",
                "Meus",
                "Voltar",
                NULL
            };
            
            while(1) {
                rl = read_scores("records.dat", fc, rl);
                show_scores(w_r, rl, -1);
                
                int selected = selection(w_l, options, COLS/2, LINES);
                
                if(strcmp(options[selected], "Todos") == 0) {
                    fc.view = ALL;
                } else if(strcmp(options[selected], "Meus") == 0) {
                    fc.view = PLAYER;
                } else if(strcmp(options[selected], "Voltar") == 0) {
                    break;
                }
            }
        } else if(key == 'd') {
            int selected_id = 0;
            
            rl = read_scores("records.dat", fc, rl);
            
            int i = 0;
            for(RecordList *element = rl; element != NULL; element = element->next)
                i++;
            
            while(1) {
                show_scores(w_r, rl, selected_id);
                
                int key = getch();
                if(key == KEY_DOWN) {
                    if(selected_id < i-1) {
                        selected_id++;
                    }
                } else if(key == KEY_UP) {
                    if(selected_id > 0) {
                        selected_id--;
                    }
                } else if(key == '\n') {
                    char *options[] = {
                        "Apagar",
                        "Cancelar",
                        NULL
                    };
                    
                    int selected = selection(stdscr, options, COLS, LINES);
                    if(strcmp(options[selected], "Apagar") == 0) {
                        RecordList *element = get_recordlist_element(rl, selected_id);
                        delete_record("records.dat", element, player);
                        break;
                    } else {
                        break;
                    }
                }
            }
            
        } else if(key == 27) {
            break;
        }
    }
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

int delete_record(char *file, RecordList *element, Player *player){
    if(strcmp(element->record.name, player->name) == 0) {
        FILE *f = fopen(file, "r+b");
        if(f != NULL) {
            Record record;
            for(fread(&record, sizeof(Record), 1, f); !feof(f); fread(&record, sizeof(Record), 1, f)) {
                if(record.deleted == 0 && strcmp(record.name, element->record.name) == 0 && record.score == element->record.score) {
                    record.deleted = 1;
                    fseek(f, -sizeof(Record), SEEK_CUR);
                    fwrite(&record, sizeof(Record), 1, f);
                    fclose(f);
                    return 1; // Conseguiu
                }
            }
            fclose(f);
            return 2; // Não encontrou
        }
        return 3; // arquivo nao existe
    }
    return 4; // Não pertence ao player
}

