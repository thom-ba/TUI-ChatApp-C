// (C) Thomas Baumeister, 2024
// For further information read the comment at the end of the file.

#ifndef SERVER_H
#define SERVER_H

// Standard Libraries
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <poll.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <signal.h>

// Custom Defines
#define BACKLOG 5
#define ADDR "127.0.0.1"
#define PORT "3490"

// Function Prototypes

void *to_sockaddr_in(struct sockaddr *sa);

void sigchld_handler(int s);

void set_sock_opt(int sock_fd);

int open_bind_socket();

int get_listener(int sock_fd);

void add_to_pfds(struct pollfd *pfds[], int new_fd, int *fd_count, int *fd_size);

void del_from_pfds(struct pollfd pfds[], int i, int *fd_count);

void ash_williams();

int send_all(int fd, char *buf, int *len);

int create_server();

#endif // SERVER_H
