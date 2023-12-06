//
//  menu_score.h
//  CTetris
//
//  Created by Emanuel on 14/11/23.
//

#ifndef menu_score_h
#define menu_score_h

#include "play.h"
#include "util.h"

typedef enum {
    CRESCENT,
    DECRESCENT
} Organizer;

typedef enum {
    BIGGERTHEN,
    SMALLERTHEN,
    CLEAR
} Filter;

typedef enum {
    ALL,
    PLAYER
} View;

typedef struct {
    Organizer name;
    Organizer score;
    Filter filter;
    View view;
    unsigned int f_value;
    Player *player;
} FilterConfig;

typedef struct recordlist {
    Record record;
    struct recordlist *next;
} RecordList;

void menu_score(Player *player);

RecordList * create_recordlist(Record record, RecordList *next);

int verify_filter(FilterConfig fc, Record record);

RecordList * read_scores(char *file, FilterConfig fc);

void show_scores(WINDOW *w, RecordList *r_list);

#endif /* menu_score_h */
