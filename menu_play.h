#include <ncurses.h>
#include "piece.h"

void menu_play(void);

void show_map(char **map, WINDOW *w);

void render_map(WINDOW *w, char **map, Piece *piece);
