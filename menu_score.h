//
//  menu_score.h
//  CTetris
//
//  Created by Emanuel on 14/11/23.
//

#ifndef menu_score_h
#define menu_score_h

#include "play.h"

typedef enum {
    CRESCENT,
    DECRESCENT
} Organizer;

typedef enum {
    BIGGER,
    SMALLER
} Filter;

typedef struct {
    Organizer name;
    Organizer score;
} FilterConfig;

typedef struct recordlist {
    Record record;
    struct recordlist *next;
} RecordList;

void menu_score(Player *player);

RecordList * create_recordlist(Record record, RecordList *next);

RecordList * read_scores(char *file, FilterConfig fc);

void show_scores(WINDOW *w, RecordList *r_list);

#endif /* menu_score_h */
