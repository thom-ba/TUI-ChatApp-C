// (C) Thomas Baumeister, 2024
// For further information read the comment at the end of the file.

#ifndef SERVER_H
#define SERVER_H

// Standard Libraries
#include <poll.h>
#include <netinet/in.h>
#include <sys/wait.h>
#include <signal.h>

#include "common.h"

// Function Prototypes
void *to_sockaddr_in(struct sockaddr *sa);

void sigchld_handler(int s);

void set_sock_opt(int sock_fd);

void open_bind_socket(int *sock_fd);

int get_listener(int sock_fd);

void add_to_pfds(struct pollfd *pfds[], int new_fd, int *fd_count, int *fd_size);

void del_from_pfds(struct pollfd pfds[], int i, int *fd_count);

void ash_williams();

int send_all(int fd, char *buf, int *len);

int create_server();

#endif // SERVER_H
