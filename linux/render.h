#ifndef RENDER_H
#define RENDER_H

#include <ncurses.h>

#define MAX_PAIRS 256

typedef struct {
    short fg, bg;
    short id;
} PairEntry;

extern PairEntry pair_cache[MAX_PAIRS];
extern int pair_count;
extern short current_fg;
extern short current_bg;
extern attr_t current_attrs;
extern short current_pair_id;

void mvfprint(WINDOW *win, int y, int x, const char *str);

#endif
