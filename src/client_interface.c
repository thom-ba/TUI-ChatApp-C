#include <string.h>

#include "term_utils.h"

const char *SUCCES_CONNECTED = "Connected successfully!";

const char *NEUTRAL_LOADING =  "Loading..";

const char *ERR_NO_SERVER = "No server was found!";


void move_cur(int row, int col) {
    printf("\033[%d;%dH", row, col);
}

int rows, cols;
void init() {
    get_term_width_height(&rows, &cols);
}

void print_loading() {
    clear_term();
    int loading_len = strlen(NEUTRAL_LOADING);

    move_cur(rows/2, (cols-loading_len)/2);
    printf("%s", NEUTRAL_LOADING); 
    fflush(stdout);
    
    clear_term();
}

void print_no_server() {
    int err_len = strlen(ERR_NO_SERVER);

    move_cur(rows/2, (cols-err_len)/2);
    printf("%s", ERR_NO_SERVER); 
    fflush(stdout);
    
    sleep(2);
    clear_term();
}

void print_connected() {
    clear_term();
    int succ_len = strlen(SUCCES_CONNECTED);

    move_cur(rows/2, (cols-succ_len)/2);
    printf("%s", SUCCES_CONNECTED);
    
    sleep(2);

    clear_term();
}
