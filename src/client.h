// (C) Thomas Baumeister, 2024
// For further information read the comment at the end of the file.

#ifndef CLIENT_H
#define CLIENT_H

#include <pthread.h>

#include "common.h"

// Function Prototypes
void msg_wuser(char** msg, const char* input);

void *handle_receive_message (void *arg);

void *handle_input (void *arg);

struct addrinfo *get_server_info ();

void network_to_string (char *s, const struct addrinfo *p);

void create_and_connect_socket (int *sock_fd);

void create_client ();

#endif // CLIENT_H
