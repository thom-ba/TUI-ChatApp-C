#ifndef TERM_UTILS_H
#define TERM_UTILS_H

#include <sys/ioctl.h>
#include <unistd.h>
#include <stdio.h>


void clear_term();
void get_term_width_height(int *rows, int *cols);

#endif // TERM_UTILS_H
