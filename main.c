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
    struct block *next;
} Block;

Block * create_block(Vector2 position, Block *next) {
    Block *b = malloc(sizeof(Block));
    b->position = position;
    b->next = next;
    return b;
}

Block * create_piece(char *content) {
    Block *p = NULL;
    unsigned int x = 0, y = 0;
    for(int i = 0; content[i] != '\0'; i++) {
        if(content[i] != '\n') {
            if(content[i] == 'b') {
                Vector2 position;
                position.x = x*2;
                position.y = y;
                p = create_block(position, p);
            }
            x++;
        } else {
            y++;
            x = 0;
        }
    }

    return p;
}

void show_piece(Block *piece, Vector2 offset, WINDOW *w) {
    for(Block *p = piece; p != NULL; p = p->next) {
        mvwprintw(w, p->position.y + offset.y, p->position.x + offset.x, "[]");
    }
}

int piece_collides(Block *piece, Vector2 offset, char **map) {
    for(Block *p = piece; p != NULL; p = p->next) {
        if(offset.y + p->position.y >= GAME_HEIGHT || map[p->position.y + offset.y][p->position.x + offset.x+1] != '.') {
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

    Block *blocks[] = {
        create_piece("bbbb"),
        create_piece("b\nbbb"),
        create_piece("bbb\nb"),
        create_piece("bb\nbb")
    };

    Block *piece = blocks[rand() % 4];

    clock_t before = clock();

    int past = 0, now = 0;

    Vector2 offset;
    offset.x = GAME_WIDTH/2;
    offset.y = 0;

    WINDOW *game_window = newwin(GAME_HEIGHT, GAME_WIDTH, GAME_POSY, GAME_POSX);

    while(1) {
        int key = getch();
        if(key == 'd') {
            offset.x+=2;
        } else if(key == 'a') {
            offset.x-=2;
        } else if(key == 's') {
            offset.y++;
        }

        clock_t diff = clock() - before;
        now = diff / CLOCKS_PER_SEC;
        if(past != now) {
            past = now;
            offset.y++;
        }

        if(piece_collides(piece, offset, map)) {
            for(Block *b = piece; b != NULL; b = b->next) {
                map[b->position.y + offset.y - 1][b->position.x + offset.x] = '[';
                map[b->position.y + offset.y - 1][b->position.x + offset.x + 1] = ']';
            }
            piece = blocks[rand() % 4];
            offset.y = 0;
            offset.x = GAME_WIDTH/2;
        }

        if(strcmp(map[GAME_HEIGHT-1], "[][][][][][][][][][]") == 0) {
            for(int i = GAME_HEIGHT-1; i > 0; i--) {
                strcpy(map[i], map[i-1]);
            }
            strcpy(map[0], "....................");
        }

        wclear(game_window);

        show_map(map, game_window);

        show_piece(piece, offset, game_window);

        wrefresh(game_window);
    }

    endwin();

    return 0;
}
