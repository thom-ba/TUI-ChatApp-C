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

// This is free and unencumbered software released into the public domain.

// Anyone is free to copy, modify, publish, use, compile, sell, or
// distribute this software, either in source code form or as a compiled
// binary, for any purpose, commercial or non-commercial, and by any
// means.

// In jurisdictions that recognize copyright laws, the author or authors
// of this software dedicate any and all copyright interest in the
// software to the public domain. We make this dedication for the benefit
// of the public at large and to the detriment of our heirs and
// successors. We intend this dedication to be an overt act of
// relinquishment in perpetuity of all present and future rights to this
// software under copyright law.

// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
// EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
// MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
// IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR
// OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
// ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
// OTHER DEALINGS IN THE SOFTWARE.

// For more information, please refer to <http://unlicense.org/>