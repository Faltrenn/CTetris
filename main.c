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

typedef struct {
    Block *blocks;
    Vector2 position;
} Piece;

typedef struct instruction {
    char *content;
    struct instruction *next;
} Instruction;

Instruction * create_instruction(char *content, Instruction *next) {
    Instruction *instruction = malloc(sizeof(Instruction));
    instruction->content = malloc((strlen(content) + 1) * sizeof(char));
    strcpy(instruction->content, content);
    instruction->next = next;
    return instruction;
}

Instruction * create_instructions(char *contents[]) {
    Instruction *instruction = NULL;
    for(int i = 0; contents[i] != NULL; i++)
        instruction = create_instruction(contents[i], instruction);

    return instruction;
}

Block * create_block(Vector2 position, Block *next) {
    Block *b = malloc(sizeof(Block));
    b->position = position;
    b->next = next;
    return b;
}

Piece * create_piece(char *instruction) {
    Piece *p = malloc(sizeof(Piece));
    p->blocks = NULL;
    p->position.x = GAME_WIDTH/4 + 1;
    p->position.y = 0;
    unsigned int x = 0, y = 0;

    for(int i = 0; instruction[i] != '\0'; i++) {
        char c = instruction[i];
        if(c != '\n') {
            if(c == 'b') {
                Vector2 position;
                position.x = x *2;
                position.y = y;
                p->blocks = create_block(position, p->blocks);
            }
            x++;
        } else {
            y++;
            x = 0;
        }
    }
    return p;
}

void show_piece(Piece *piece, WINDOW *w) {
    for(Block *b = piece->blocks; b != NULL; b = b->next) {
        mvwprintw(w, b->position.y + piece->position.y, b->position.x + piece->position.x, "[]");
    }
}

int piece_collides(Piece *piece, char **map) {
    for(Block *b = piece->blocks; b != NULL; b = b->next) {
        int posx = b->position.x + piece->position.x;
        int posy = b->position.y + piece->position.y;
        if(posy >= GAME_HEIGHT || map[posy][posx] != '.')
            return 1;
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

void move_piece(Piece *piece, Vector2 direction, char **map) {
    for(Block *b = piece->blocks; b != NULL; b = b->next) {
        int new_posx = (b->position.x + piece->position.x) + 2 * direction.x;
        if(new_posx < 0 || new_posx >= GAME_WIDTH || map[b->position.y + piece->position.y][new_posx] != '.')
            return;
    }
    piece->position.x += direction.x * 2;
    piece->position.y += direction.y;
}

Block * copy_blocks(Block * blocks) {
    Block *copy = malloc(sizeof(Block));
    Block *aux = copy;
    for(Block *b = blocks; b != NULL; b = b->next) {
        if(b != NULL) {
            *aux = *b;
            aux->next = malloc(sizeof(Block));
            aux = aux->next;
        } else {
            aux->next = NULL;
        }
    }
    return copy;
}

void rotate_piece(Piece *piece, char **map) {
    Block *backup = copy_blocks(piece->blocks);
    int i = 0, lasty = piece->blocks->position.y;
    for(Block *b = piece->blocks; b != NULL; b = b->next) {
        if(lasty != b->position.y) {
            i++;
            lasty = b->position.y;
        }
        Vector2 new_position;
        new_position.y = b->position.x/2;
        new_position.x = i*2;
        b->position = new_position;
    }

    if(piece_collides(piece, map)) {
        piece->blocks = backup;
    } else {
        Block *new_blocks = NULL;

        int y = 0;
        while(piece->blocks != NULL) {
            Block *last = NULL;
            Block *now = piece->blocks;
            while(now != NULL) {
                if(now->position.y == y) {
                    new_blocks = create_block(now->position, new_blocks);
                    if(last == NULL) {
                        piece->blocks = piece->blocks->next;
                    } else {
                        last->next = now->next;
                    }
                    now = now->next;
                } else {
                    last = now;
                    now = now->next;
                }
            }
            y++;
        }
        piece->blocks = new_blocks;
    }
}

void get_piece(Piece *out, char *options[]) {
    *out = *create_piece(options[rand() % 6]);
}

void render(WINDOW *w, char **map, Piece *piece) {
    wclear(w);

    show_map(map, w);

    show_piece(piece, w);

    wrefresh(w);
}

void down_piece(Piece * piece, char **map, char *options[], WINDOW *w) {
    Vector2 direction;
    direction.x = 0;
    direction.y = 1;
    while(1) {
        move_piece(piece, direction, map);

        if(piece_collides(piece, map)) {
            for(Block *b = piece->blocks; b != NULL; b = b->next) {
                map[b->position.y + piece->position.y - 1][b->position.x + piece->position.x] = '[';
                map[b->position.y + piece->position.y - 1][b->position.x + piece->position.x + 1] = ']';
            }
            get_piece(piece, options);
            break;
        }

        render(w, map, piece);

        usleep(10000/2);
    }
}

int main() {
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

    WINDOW *game_window = newwin(GAME_HEIGHT, GAME_WIDTH, GAME_POSY, GAME_POSX);

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
                break;
            }
        }

        for(int i = GAME_HEIGHT-1; i > 0; i--) {
            if(strcmp(map[i], "[][][][][][][][][][]") == 0) {
                for(int j = i; j > 0; j--) {
                    strcpy(map[j], map[j-1]);
                }
                strcpy(map[0], "....................");
            }
        }

        render(game_window, map, piece);
    }

    endwin();

    return 0;
}
