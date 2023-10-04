#include <stdio.h>
#include <curses.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>

#define GAME_WIDTH COLS
#define GAME_HEIGHT LINES

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

void show_piece(Block *piece, Vector2 offset) {
    for(Block *p = piece; p != NULL; p = p->next) {
        mvprintw(p->position.y + offset.y, p->position.x + offset.x, "[]");
    }
}

int piece_collides(Block *piece, Vector2 offset) {
    for(Block *p = piece; p != NULL; p = p->next) {
        if(offset.y + p->position.y >= GAME_HEIGHT) {
            return 1;
        }
    }
    return 0;
}

int main() {
    initscr();
    cbreak();
    noecho();
    nodelay(stdscr, 1);
    curs_set(0);

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
    offset.x = GAME_WIDTH/2 - 1;
    offset.y = 0;

    while(1) {
        clear();

        clock_t diff = clock() - before;
        now = diff / CLOCKS_PER_SEC;
        if(past != now) {
            past = now;
            offset.y++;
        }

        if(piece_collides(piece, offset)) {
            piece = blocks[rand() % 4];
            offset.y = 0;
            offset.x = GAME_WIDTH/2 -1;
        }

        show_piece(piece, offset);

        refresh();

        int key = getch();
        if(key == 'd') {
            offset.x++;
        } else if(key == 'a') {
            offset.x--;
        } else if(key == 's') {
            offset.y++;
        }
    }

    endwin();

    return 0;
}
