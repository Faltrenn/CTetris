#include <ncurses.h>
#include <time.h>
#include <string.h>
#include <stdlib.h>
#include "play.h"
#include "piece.h"

void show_map(char **map, WINDOW *w) {
    for(int i = 0; i < GAME_HEIGHT; i++) {
        for(int j = 0; j < GAME_WIDTH; j++) {
            mvwprintw(w, i, 0, "%s", map[i]);
        }
    }
}

void render_map(WINDOW *w, char **map, Piece *piece) {
    wclear(w);

    show_map(map, w);

    show_piece(piece, w);

    wrefresh(w);
}

void save_record(Record record) {
    FILE *save_at = fopen("records", "ab");
    fwrite(&record, sizeof(Record), 1, save_at);
    fclose(save_at);
}

void play(void) {
    initscr();
    cbreak();
    noecho();
    nodelay(stdscr, 1);
    curs_set(0);

    keypad(stdscr, TRUE);

    srand(time(NULL));

    char **map = malloc(GAME_HEIGHT * sizeof(char **));
    for(int i = 0; i < GAME_HEIGHT; i++) {
        map[i] = malloc((GAME_WIDTH+1) * sizeof(char *));
        strcpy(map[i], "....................");
    }

    char *options[] = {
        "bbbb",
        "b\nbbb",
        "  b\nbbb",
        " b\nbbb",
        "bb\nbb",
        " bb\nbb",
        "bb\n bb",
    };

    Piece *piece = malloc(sizeof(Piece));
    get_piece(piece, options);

    clock_t before = clock();

    int past = 0, now = 0;

    char *name = "Opa";
    Record record;
    for(int i = 0; i < 3; i++) {
        record.name[i] = name[i];
    }
    record.name[3] = '\0';
    record.score = 0;

    WINDOW *game_window = newwin(GAME_HEIGHT, GAME_WIDTH, GAME_POSY, GAME_POSX);

    WINDOW *infos = newwin(INFO_HEIGHT, INFO_WIDTH, INFO_POSY, INFO_POSX);
    
    wclear(infos);
    mvwprintw(infos, 0, 0, "Pontuacao: %d", record.score);
    wrefresh(infos);

    while(1) {
        Vector2 direction;
        direction.x = 0;
        direction.y = 0;

        int key = getch();
        if(key == KEY_RIGHT) {
            direction.x = 1;
        } else if(key == KEY_LEFT) {
            direction.x = -1;
        } else if(key == KEY_DOWN) {
            direction.y = 1;
        } else if(key == ' ') {
            down_piece(piece, map, options, game_window);
        } else if(key == 'r') {
            rotate_piece(piece, map);
        }

        clock_t diff = clock() - before;
        now = diff / CLOCKS_PER_SEC;
        if(past != now) {
            past = now;
            direction.y = 1;
        }

        move_piece(piece, direction, map);

        if(piece_collides(piece, map)) {
            for(Block *b = piece->blocks; b != NULL; b = b->next) {
                map[b->position.y + piece->position.y - 1][b->position.x + piece->position.x] = '[';
                map[b->position.y + piece->position.y - 1][b->position.x + piece->position.x + 1] = ']';
            }
            get_piece(piece, options);
            if(piece_collides(piece, map)) {
                gameover(record);
                break;
            }
        }

        for(int i = GAME_HEIGHT-1; i > 0; i--) {
            if(strcmp(map[i], "[][][][][][][][][][]") == 0) {
                for(int j = i; j > 0; j--) {
                    strcpy(map[j], map[j-1]);
                }
                strcpy(map[0], "....................");
                record.score++;

                wclear(infos);
                mvwprintw(infos, 0, 0, "Pontuacao: %d", record.score);
                wrefresh(infos);
            }
        }

        render_map(game_window, map, piece);
    }

    endwin();
}

void gameover(Record record) {
    char name[4] = {'_', '_', '_', '\0'};
    nodelay(stdscr, 0);
    curs_set(1);
    keypad(stdscr, 1);

    int i = 0;
    while(i < 3) {
        clear();

        mvprintw(0,0, "Name: %s", name);
        
        move(0, 6+i);

        int key = getch();
        if(key >= 'a' && key <= 'z') {
            name[i] = key;
            i++;
        } else if(key == 127) {
            if (i > 0) {
                i--;
                name[i] = '_';
            }
        }

        refresh();
    }

    curs_set(0);
    name[3] = '\0';

    strcpy(record.name, name);

    save_record(record);
}
