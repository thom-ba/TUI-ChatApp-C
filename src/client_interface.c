#include <string.h>

#include "term_utils.h"

const char *SUCCES_CONNECTED = "Connected successfully!";

const char *NEUTRAL_LOADING =  "Loading..";
const char *NEUTRAL_CREATE_CLIENT = "Please choose name: ";

const char *ERR_NO_SERVER = "No server was found!";


void move_cur(int row, int col) {
    printf("\033[%d;%dH", row, col);
}

int rows, cols;
void init() {
    get_term_width_height(&rows, &cols);
}

char* print_create_user()  {
	char username[26];
	
	clear_term();
	int create_user_length = strlen(NEUTRAL_CREATE_CLIENT);
	
	move_cur(rows/2, (cols-create_user_length)/2-10);
	printf("%s", NEUTRAL_CREATE_CLIENT);
	fflush(stdout);
	
	//TODO: wait for username
	fgets(username, sizeof(username), stdin);
	
	printf("Username: %s", username);

	clear_term();
	
	return username;
}

void print_room() {
	
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
	fflush(stdout);
    
    sleep(1);

    clear_term();
}
