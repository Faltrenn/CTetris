#include <stdio.h>
#include <curses.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>

#define GAME_WIDTH (10 * 2)
#define GAME_HEIGHT LINES
#define GAME_POSX (COLS/2 - GAME_WIDTH/2)
#define GAME_POSY 0

typedef struct {
    unsigned int x;
    unsigned int y;
} Vector2;

typedef struct block {
    Vector2 position;
    char *instructions;
    struct block *next;
} Block;

Block * create_block(Vector2 position, char *instructions, Block *next) {
    Block *b = malloc(sizeof(Block));
    b->position = position;
    b->instructions = instructions;
    b->next = next;
    return b;
}

Block * create_piece(char *content) {
    Block *p = NULL;
    unsigned int x = 0, y = 0, offset = GAME_WIDTH/4;

    for(int i = 0; content[i] != '\0'; i++) {
        if(content[i] != '\n') {
            if(content[i] == 'b') {
                Vector2 position;
                position.x = (x + offset)*2;
                position.y = y;
                p = create_block(position, content, p);
            }
            x++;
        } else {
            y++;
            x = 0;
        }
    }

    return p;
}

void show_piece(Block *piece, WINDOW *w) {
    for(Block *p = piece; p != NULL; p = p->next) {
        mvwprintw(w, p->position.y, p->position.x, "[]");
    }
}

int piece_collides(Block *piece, char **map) {
    for(Block *p = piece; p != NULL; p = p->next) {
        if(p->position.y >= GAME_HEIGHT || map[p->position.y][p->position.x] != '.') {
            return 1;
        }
    }
    return 0;
}

void show_map(char **map, WINDOW *w) {
    for(int i = 0; i < GAME_HEIGHT; i++) {
        for(int j = 0; j < GAME_WIDTH; j++) {
            mvwprintw(w, i, 0, "%s", map[i]);
        }
    }
}

void move_piece(Block *piece, Vector2 direction, char **map) {
    for(Block *p = piece; p != NULL; p = p->next) {
        int new_posx = p->position.x + 2 * direction.x;
        if(new_posx < 0 || new_posx >= GAME_WIDTH || map[p->position.y][new_posx] != '.') {
            return;
        }
    }
    for(Block *p = piece; p != NULL; p = p->next) {
        p->position.x += 2 * direction.x;
        p->position.y += direction.y;
    }
}

void get_piece(Block *out, char *options[]) {
    *out = *create_piece(options[rand() % 4]);
}

int main() {
    initscr();
    cbreak();
    noecho();
    nodelay(stdscr, 1);
    curs_set(0);

    char **map = malloc(GAME_HEIGHT * sizeof(char **));
    for(int i = 0; i < GAME_HEIGHT; i++) {
        map[i] = malloc((GAME_WIDTH+1) * sizeof(char *));
        strcpy(map[i], "....................");
    }

    char *blocks[] = {
        "bbbb",
        "b\nbbb",
        "bbb\nb",
        "bb\nbb"
    };

    Block *piece = malloc(sizeof(Block));
    get_piece(piece, blocks);

    clock_t before = clock();

    int past = 0, now = 0;

    WINDOW *game_window = newwin(GAME_HEIGHT, GAME_WIDTH, GAME_POSY, GAME_POSX);

    while(1) {
        Vector2 direction;
        direction.x = 0;
        direction.y = 0;

        int key = getch();
        if(key == 'd') {
            direction.x = 1;
        } else if(key == 'a') {
            direction.x = -1;
        } else if(key == 's') {
            direction.y = 1;
        }

        clock_t diff = clock() - before;
        now = diff / CLOCKS_PER_SEC;
        if(past != now) {
            past = now;
            direction.y = 1;
        }

        move_piece(piece, direction, map);

        if(piece_collides(piece, map)) {
            for(Block *b = piece; b != NULL; b = b->next) {
                map[b->position.y - 1][b->position.x] = '[';
                map[b->position.y - 1][b->position.x + 1] = ']';
            }
            get_piece(piece, blocks);
        }

        for(int i = GAME_HEIGHT-1; i > 0; i--) {
            if(strcmp(map[i], "[][][][][][][][][][]") == 0) {
                for(int j = i; j > 0; j--) {
                    strcpy(map[j], map[j-1]);
                }
                strcpy(map[0], "....................");
            }
        }

        wclear(game_window);

        show_map(map, game_window);

        show_piece(piece, game_window);

        wrefresh(game_window);
    }

    endwin();

    return 0;
}
