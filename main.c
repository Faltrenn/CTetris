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
    unsigned int id;
    struct block *next;
} Block;

typedef struct {
    unsigned int rot;
    Block *blocks;
    unsigned int n;
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

Block * create_block(Vector2 position, int id, Block *next) {
    Block *b = malloc(sizeof(Block));
    b->position = position;
    b->id = id;
    b->next = next;
    return b;
}

Piece * create_piece(Instruction *instruction) {
    Piece *p = malloc(sizeof(Piece));
    p->blocks = NULL;
    p->rot = 0;
    unsigned int x = 0, y = 0, offset = GAME_WIDTH/4, n=0;

    for(Instruction *instruct = instruction; instruct != NULL; instruct = instruct->next) {
        x = 0;
        y = 0;
        for(int i = 0; instruct->content[i] != '\0'; i++) {
            char c = instruct->content[i];
            if(c != '\n') {
                if(c == 'b') {
                    Vector2 position;
                    position.x = (x + offset)*2;
                    position.y = y;
                    p->blocks = create_block(position, n, p->blocks);
                }
                x++;
            } else {
                y++;
                x = 0;
            }
        }
        n++;
    }
    p->n = n;
    return p;
}

void show_piece(Piece *piece, WINDOW *w) {
    for(Block *b = piece->blocks; b != NULL; b = b->next) {
        if(b->id == piece->rot)
            mvwprintw(w, b->position.y, b->position.x, "[]");
    }
}

int piece_collides(Piece *piece, char **map) {
    for(Block *b = piece->blocks; b != NULL; b = b->next) {
        if(b->id == piece->rot)
            if(b->position.y >= GAME_HEIGHT || map[b->position.y][b->position.x] != '.')
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
        int new_posx = b->position.x + 2 * direction.x;
        if(b->id == piece->rot)
            if(new_posx < 0 || new_posx >= GAME_WIDTH || map[b->position.y][new_posx] != '.')
                return;
    }
    for(Block *b = piece->blocks; b != NULL; b = b->next) {
        b->position.x += 2 * direction.x;
        b->position.y += direction.y;
    }
}

void get_piece(Piece *out, Instruction *options[]) {
    *out = *create_piece(options[rand() % 4]);
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

    Instruction **instructions = malloc(4 * sizeof(Instruction));
    instructions[0] = create_instructions((char*[]){"bbbb", "b\nb\nb\nb", NULL});
    instructions[1] = create_instructions((char*[]){" b\n b\nbb", "bbb\n  b", "bb\nb\nb", "b\nbbb", NULL});
    instructions[2] = create_instructions((char*[]){"  b\nbbb", "bb\n b\n b", "bbb\nb", "b\nb\nbb", NULL});
    instructions[3] = create_instructions((char*[]){"bb\nbb", NULL});

    Piece *piece = malloc(sizeof(Piece));
    get_piece(piece, instructions);

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
        } else if(key == 'r') {
            piece->rot++;
            if(piece->rot >= piece->n)
                piece->rot = 0;
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
                if(b->id == piece->rot) {
                    map[b->position.y - 1][b->position.x] = '[';
                    map[b->position.y - 1][b->position.x + 1] = ']';
                }
            }
            get_piece(piece, instructions);
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

        wclear(game_window);

        show_map(map, game_window);

        show_piece(piece, game_window);

        wrefresh(game_window);
    }

    endwin();

    return 0;
}
