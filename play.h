#ifndef play_h
#define play_h

#include <ncurses.h>
#include "piece.h"
#include "util.h"

void play(Player *player);

void show_map(char **map, WINDOW *w);

void render_map(WINDOW *w, char **map, Piece *piece);

typedef struct {
    char name[7];
    unsigned int score;
    unsigned int deleted;
} Record;

void gameover(Record record);

void save_record(Record *record);

#endif
