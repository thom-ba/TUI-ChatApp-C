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
int buf_size = 10;
char** msg_buf = NULL;

void
print_room(char* msg)
{
    clear_term();
 
    if(msg_buf == NULL) {
        msg_buf = malloc(buf_size * sizeof(char*));
        
        // Initialize the buffer to NULL
        for (int i = 0; i < buf_size; ++i) {
            msg_buf[i] = malloc(56 * sizeof(char));
        }
    }
    
    // Display all messages
    if(!is_emptyy(msg)) {
        msg[strlen(msg)] = '\0';
        // move_cur(msg_count, 0);
        msg_buf[msg_count] = strdup(msg);
        
        for (int i = 0; i < sizeof(msg_buf); i++)  {
            printf("%s", msg_buf[i]);
        }
        
        fflush(stdout);
        msg_count++;
    }
    
    // Display bottom input line
    move_cur(rows-2, 0);
    
    for(int i = 0; i < cols; i++) {
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
