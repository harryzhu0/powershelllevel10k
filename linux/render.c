#include <ncurses.h>
#include <ctype.h>
#include "render.h"

PairEntry pair_cache[MAX_PAIRS];
int pair_count = 0;
short current_fg = -1;
short current_bg = -1;
attr_t current_attrs = A_NORMAL;
short current_pair_id = 0;


static short map_ansi_8color_fg(int code) {
    // 30–37, 90–97
    if (code >= 30 && code <= 37) return code - 30;
    if (code >= 90 && code <= 97) return code - 90;
    return -1;
}

static short map_ansi_8color_bg(int code) {
    // 40–47, 100–107
    if (code >= 40 && code <= 47) return code - 40;
    if (code >= 100 && code <= 107) return code - 100;
    return -1;
}  // or i could add 10 to the fg


static short get_pair_id(short fg, short bg) {
    // reuse if exists
    for (int i = 0; i < pair_count; i++) {
        if (pair_cache[i].fg == fg && pair_cache[i].bg == bg)
            return pair_cache[i].id;
    }
    if (pair_count >= MAX_PAIRS - 1) {
        // fallback: just use first pair
        return pair_cache[0].id;
    }
    short id = (short)(pair_count + 1); // pair 1..N
    init_pair(id, fg, bg);
    pair_cache[pair_count].fg = fg;
    pair_cache[pair_count].bg = bg;
    pair_cache[pair_count].id = id;
    pair_count++;
    return id;
}

static int parse_int(const char **p) {
    int v = 0;
    while (isdigit((unsigned char)**p)) {
        v = v * 10 + (**p - '0');
        (*p)++;
    }
    return v;
}

static void apply_state(WINDOW *win) {
    if (current_pair_id > 0) {
        wattrset(win, current_attrs | COLOR_PAIR(current_pair_id));
    } else {
        wattrset(win, current_attrs);
    }
}

void mvfprint(WINDOW *win, int y, int x, const char *str) {
    current_fg = -1;
    current_bg = -1;
    current_attrs = A_NORMAL;
    current_pair_id = 0;
    apply_state(win);

    wmove(win, y, x);

    while (*str) {
        if (*str == '\n') {
            int cy, cx;
            getyx(win, cy, cx);
            wmove(win, cy + 1, 0);
            str++;
            continue;
        }

        if (*str == '\033' && *(str + 1) == '[') {
            str += 2; // skip ESC[

            // collect all SGR params until 'm'
            int params[16];
            int nparams = 0;

            if (*str == 'm') {
                // bare ESC[m → same as 0
                params[nparams++] = 0;
                str++;
            } else {
                while (*str && *str != 'm' && nparams < 16) {
                    params[nparams++] = parse_int(&str);
                    if (*str == ';') str++;
                }
                if (*str == 'm') str++;
            }

            // process params
            for (int i = 0; i < nparams; i++) {
                int p = params[i];

                if (p == 0) {
                    // reset
                    current_fg = -1;
                    current_bg = -1;
                    current_attrs = A_NORMAL;
                    current_pair_id = 0;
                    apply_state(win);
                } else if (p == 1) {
                    current_attrs |= A_BOLD;
                    apply_state(win);
                } else if (p == 2) {
                    current_attrs |= A_DIM;
                    apply_state(win);
                } else if (p == 4) {
                    current_attrs |= A_UNDERLINE;
                    apply_state(win);
                } else if (p == 22) {
                    current_attrs &= ~A_BOLD;
                    current_attrs &= ~A_DIM;
                    apply_state(win);
                } else if (p == 24) {
                    current_attrs &= ~A_UNDERLINE;
                    apply_state(win);
                } else if (p == 39) {
                    current_fg = -1;
                    current_pair_id = (current_bg != -1)
                        ? get_pair_id(current_fg, current_bg)
                        : 0;
                    apply_state(win);
                } else if (p == 49) {
                    current_bg = -1;
                    current_pair_id = (current_fg != -1)
                        ? get_pair_id(current_fg, current_bg)
                        : 0;
                    apply_state(win);
                } else if (p == 38 || p == 48) {
                    // 38;5;X or 48;5;X
                    int is_fg = (p == 38);
                    if (i + 2 < nparams && params[i + 1] == 5) {
                        int color = params[i + 2];
                        if (is_fg) current_fg = (short)color;
                        else       current_bg = (short)color;
                        i += 2;
                        if (current_fg != -1 || current_bg != -1) {
                            current_pair_id = get_pair_id(current_fg, current_bg);
                            apply_state(win);
                        }
                    }
                } else if ((p >= 30 && p <= 37) || (p >= 90 && p <= 97)) {
                    short fg = map_ansi_8color_fg(p);
                    if (fg != -1) {
                        current_fg = fg;
                        current_pair_id = get_pair_id(current_fg, current_bg);
                        apply_state(win);
                    }
                } else if ((p >= 40 && p <= 47) || (p >= 100 && p <= 107)) {
                    short bg = map_ansi_8color_bg(p);
                    if (bg != -1) {
                        current_bg = bg;
                        current_pair_id = get_pair_id(current_fg, current_bg);
                        apply_state(win);
                    }
                }
            }
        } else {
            waddch(win, (unsigned char)*str++);
        }
    }
}
