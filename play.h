#include <ncurses.h>
#include "piece.h"

void play(void);

void show_map(char **map, WINDOW *w);

void render_map(WINDOW *w, char **map, Piece *piece);

typedef struct {
    char name[4];
    unsigned int score;
} Record;

void gameover(Record record);

void save_record(Record record);
