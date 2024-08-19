#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "term_utils.h"


const char *SUCCES_CONNECTED = "Connected successfully!";

const char *NEUTRAL_LOADING = "Loading..";
const char *NEUTRAL_CREATE_CLIENT = "Please choose name: ";

const char *ERR_NO_SERVER = "No server was found!";

void
move_cur (int row, int col)
{
  printf ("\033[%d;%dH", row, col);
}

int rows, cols;
void
init ()
{
  get_term_width_height (&rows, &cols);
}

char *
print_create_user ()
{
  char buffer[48];

  clear_term ();
  int create_user_length = strlen (NEUTRAL_CREATE_CLIENT);

  move_cur (rows / 2, (cols - create_user_length) / 2 - 10);
  printf ("%s", NEUTRAL_CREATE_CLIENT);
  fflush (stdout);

  fgets (buffer, sizeof (buffer), stdin);

  buffer[strcspn (buffer, "\n")] = '\0';

  char *username = malloc (strlen (buffer) + 1);

  strcpy (username, buffer);

  clear_term ();

  return username;
}

bool
is_emptyy(const char* str) {
	return str[0] == '\0';
}

#define NEUTRAL_BOX_CHAR "─"
int msg_count = 0;
int buff_size = 100;
char** msg_buf = NULL;

void shift_buffer(char** buff) {
    memmove(buff, buff +1, msg_count -1 * sizeof(char*));
}

void print_buffer(char** buf) {
    for(int i = 0; i < msg_count; i++) {
        printf("%s", buf[i]);
    }
    printf("\n");
    fflush(stdout);
}

void print_room(char* msg) {
    buff_size = rows;
    
    clear_term();

    if (msg_buf == NULL) {
        msg_buf = malloc(buff_size * sizeof(char*));
        for (int i = 0; i < buff_size; ++i) {
            msg_buf[i] = NULL;
        }
    }

    msg_buf[msg_count] = strdup(msg);
    msg_count++;

    if (msg_count == buff_size) {
        free(msg_buf[0]);
        memmove(msg_buf, msg_buf + 1, (buff_size - 1) * sizeof(char*));
        msg_count--;
    }
    
    print_buffer(msg_buf);
    
    // Print bottom input line
    move_cur(rows - 1, 0);
    for (int i = 0; i < cols; i++) {
        printf(NEUTRAL_BOX_CHAR);
    }
    printf("\n");
    fflush(stdout);

    printf("Enter message: ");
    
}

void
print_loading ()
{
  clear_term ();
  int loading_len = strlen (NEUTRAL_LOADING);

  move_cur (rows / 2, (cols - loading_len) / 2);
  printf ("%s", NEUTRAL_LOADING);
  fflush (stdout);

  clear_term ();
}

void
print_no_server ()
{
  int err_len = strlen (ERR_NO_SERVER);

  move_cur (rows / 2, (cols - err_len) / 2);
  printf ("%s", ERR_NO_SERVER);
  fflush (stdout);

  sleep (2);
  clear_term ();
}

void
print_connected ()
{
  clear_term ();
  int succ_len = strlen (SUCCES_CONNECTED);

  move_cur (rows / 2, (cols - succ_len) / 2);
  printf ("%s", SUCCES_CONNECTED);
  fflush (stdout);

  usleep(500000);

  clear_term ();
}
