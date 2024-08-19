// (C) Thomas Baumeister, 2024
// For further information read the comment at the end of the file.

#ifndef SERVER_H
#define SERVER_H

// Standard Libraries
#include <netinet/in.h>
#include <poll.h>
#include <signal.h>
#include <sys/wait.h>

#include "common.h"

#define FD_START_SIZE 5
#define POLL_TIMEOUT -1

// Structs
typedef struct
{
  int fd_count;
  int fd_size;
  struct pollfd *pfds;
} Poll_Info;

// Function Prototypes
void *to_sockaddr_in (struct sockaddr *sa);

void sigchld_handler ();

void set_sock_opt (int sock_fd);

void open_bind_socket (int *sock_fd);

int get_listener (int sock_fd);

void add_to_pfds (Poll_Info *pi, int new_fd);

void del_from_pfds (struct pollfd pfds[], int i, int *fd_count);

void ash_williams ();

int send_all (int fd, char *buf, int *len);

int create_server ();

#endif // SERVER_H
