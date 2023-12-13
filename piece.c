#include <string.h>
#include <time.h>
#include <unistd.h>
#include <stdlib.h>
#include "play.h"
#include "piece.h"

Block * create_block(Vector2 position, Block *next) {
    Block *b = malloc(sizeof(Block));
    b->position = position;
    b->next = next;
    return b;
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

void move_piece(Piece *piece, Vector2 direction, char **map) {
    for(Block *b = piece->blocks; b != NULL; b = b->next) {
        int new_posx = (b->position.x + piece->position.x) + 2 * direction.x;
        if(new_posx < 0 || new_posx >= GAME_WIDTH || map[b->position.y + piece->position.y][new_posx] != '.')
            return;
    }
    piece->position.x += direction.x * 2;
    piece->position.y += direction.y;
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

void free_blocks(Block *blocks) {
    if(blocks->next != NULL)
        free_blocks(blocks->next);
    free(blocks);
}

void free_piece(Piece *piece) {
    if(piece != NULL && piece->blocks != NULL) {
        free_blocks(piece->blocks);
        free(piece);
    }
}

Piece * get_piece(Piece *out, char *options[]) {
    free_piece(out);
    return create_piece(options[rand() % 7]);
}

Piece * down_piece(Piece * piece, char **map, char *options[], WINDOW *w) {
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
            return get_piece(piece, options);
            break;
        }

        render_map(w, map, piece);

        usleep(10000/2);
    }
}

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
