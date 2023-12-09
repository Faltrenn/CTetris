#include <ncurses.h>
#include "game_info.h"

#ifndef PIECE_H
#define PIECE_H

typedef struct {
    unsigned int x;
    unsigned int y;
} Vector2;

typedef struct block {
    Vector2 position;
    struct block *next;
} Block;

Block * create_block(Vector2 position, Block *next);
Block * copy_blocks(Block * blocks);

typedef struct {
    Block *blocks;
    Vector2 position;
} Piece;

Piece * create_piece(char *instruction);
void show_piece(Piece *piece, WINDOW *w);
int piece_collides(Piece *piece, char **map);
void move_piece(Piece *piece, Vector2 direction, char **map);
void rotate_piece(Piece *piece, char **map);
Piece * down_piece(Piece * piece, char **map, char *options[], WINDOW *w);
Piece * get_piece(Piece *out, char *options[]);

typedef struct instruction {
    char *content;
    struct instruction *next;
} Instruction;

Instruction * create_instruction(char *content, Instruction *next);
Instruction * create_instructions(char *contents[]);
void free_piece(Piece *piece);
void free_blocks(Block *block);

#endif
