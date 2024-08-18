#ifndef TERM_UTILS_H
#define TERM_UTILS_H

#include <stdio.h>
#include <sys/ioctl.h>
#include <unistd.h>

void clear_term ();
void get_term_width_height (int *rows, int *cols);

#endif // TERM_UTILS_H
