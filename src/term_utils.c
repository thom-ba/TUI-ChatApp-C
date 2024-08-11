#include "term_utils.h"

const char *CLEAR_SCREEN = "\033[2J\033[H";

void clear_term() {
    printf("%s", CLEAR_SCREEN);
}

void get_term_width_height(int *rows, int *cols) {
    struct winsize w;
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);

    *rows = w.ws_row;
    *cols = w.ws_col;
}

