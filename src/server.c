//   (C) Thomas Baumeister, 2024  : "4
//   For more information, see the public domain dedication at the end of this file.

#include "server.h"

void *to_sockaddr_in(struct sockaddr *sa) {
    return &(((struct sockaddr_in*)sa)->sin_addr);
}

void sigchld_handler(int s) {
    int saved_errno = errno;

    while (waitpid(-1, NULL, WNOHANG) > 0);

    errno = saved_errno;
}

void set_sock_opt(int sock_fd)  {
    int yes = 1;

    if(setsockopt(sock_fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof yes) == -1) {
        perror("setsocketopt");
        exit(1);
    }
}

void open_bind_socket(int *sock_fd) {
    struct addrinfo hints, *res;

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    getaddrinfo(ADDR, PORT, &hints, &res);
    *sock_fd = socket(res->ai_family, res->ai_socktype, res->ai_protocol); 

    set_sock_opt(*sock_fd);

    bind(*sock_fd, res->ai_addr, res->ai_addrlen);

    free(res);
}

int get_listener(int sock_fd) {
    listen(sock_fd, BACKLOG); 

    return sock_fd;
}

void add_to_pfds(struct pollfd *pfds[], int new_fd, int *fd_count, int *fd_size) { 
    if (*fd_count == *fd_size) {
        *fd_size *= 2;

        *pfds = realloc(*pfds, sizeof(**pfds) * (*fd_size));
    }

    (*pfds)[*fd_count].fd = new_fd;
    (*pfds)[*fd_count].events = POLLIN;

    (*fd_count)++;
}

void del_from_pfds(struct pollfd pfds[], int i, int *fd_count) {
    pfds[i] = pfds[*fd_count-1];

    (*fd_count)--;
}

// Ash Williams is the sigchld_handler and will clean up zombies
void ash_williams() {
    struct sigaction sa;

    printf("Ash Williams: Does that sound fine?\n");

    sa.sa_handler = sigchld_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;
    if (sigaction(SIGCHLD, &sa, NULL) == -1) {
        perror("sigaction");
        exit(1);
    } 
}

int send_all(int fd, char *buf, int *len) {
    int total_sent = 0;
    int bytes_left = *len;
    int n;

    while(total_sent < *len) {
        if((n = send(fd, buf + total_sent, bytes_left, 0)) == -1) {
            break;
        }

        printf("Server: sent %d bytes\n", n);

        total_sent += n;
        bytes_left -= n;
    }

    *len = total_sent;

    return n==-1?-1:0;
}

// TODO: BIG TODO: This is a mess, needs to be cleaned up
void on_poll(int sock_fd, struct pollfd *pfds, int index, int fd_count, int fd_size) {
    char client_ip[INET6_ADDRSTRLEN];
    struct sockaddr_storage client_addr;
    int listener = get_listener(sock_fd);
    socklen_t addr_len = sizeof(client_addr);
    char buf[256];

    int new_fd; 
    if (pfds[index].fd == listener) { // Is our listener
        if ((new_fd = accept(listener, (struct sockaddr *)&client_addr, &addr_len) ) == -1) {
            perror("accept");
        }
        else {
            add_to_pfds(&pfds, new_fd, &fd_count, &fd_size);

            printf("Pollserver: new connection from %s on socket %d\n", 
                    inet_ntop(client_addr.ss_family, 
                        to_sockaddr_in((struct sockaddr*) &client_addr),
                        client_ip,
                        INET6_ADDRSTRLEN),
                    new_fd
                  );

            if (send(new_fd, "Welcome!", 8, 0) == -1) {
                perror("welcome");
            }
        }
    } else {
        memset(buf, 0, sizeof(buf)); 
        int nbytes = recv(pfds[index].fd, buf, sizeof buf, 0);
        int sender_fd = pfds[index].fd;

        if (nbytes == 0) { // Got an error or connection closed
            if (nbytes <= 0) {
                printf("Pollserver: socket %d hung up\n", sender_fd);
            } else {
                perror("recv");
            }

            close(sender_fd);
            del_from_pfds(pfds, index, &fd_count);

        } else { // We got data from client
            for (int j = 0; j < fd_count; j++) {
                int dst_fd = pfds[j].fd;

                // Except the listener and ourselves
                if (dst_fd != listener && dst_fd != sender_fd) {
                    printf("Debug: nbytes: %d\n", nbytes);
                    printf("Debug: buf: %s\n", buf);
                    if (send(dst_fd, buf, nbytes, 0) == -1) {
                        perror("send");
                    }
                }

                // need to clear the buffer, otherwise we just overwrite it the next time 
            } 
        }
    } // END handle data from poll
}

void init_poll_info(Poll_Info *pi) {
    pi->fd_count = 0;
    pi->fd_size = FD_START_SIZE;
    pi->pfds = malloc(sizeof *pi->pfds * pi->fd_size);
}

int create_server() {
    Poll_Info poll_info;
    init_poll_info(&poll_info);

    int sock_fd;
    int listener;

    open_bind_socket(&sock_fd);
    listener = get_listener(sock_fd);    
    ash_williams();

    poll_info.pfds[0].fd = listener;
    poll_info.pfds[0].events = POLLIN;
    poll_info.fd_count = 1;

    printf("Server: Waiting for connections on: %s ...\n", ADDR);

    for(;;)
    {
        int poll_count = poll(poll_info.pfds, poll_info.fd_count, -1);
        if (poll_count == -1) {
            perror("poll");
            exit(1);
        }

        for (int i = 0; i < poll_info.fd_count; i++) { // Loop through all file_descriptors
            if (poll_info.pfds[i].revents & POLLIN) { // We got an event
              on_poll(sock_fd, poll_info.pfds, i, poll_info.fd_count, poll_info.fd_size); 
               
            } // END got an event
        } // END looping through file descriptors
    } // END for(;;)
}

int main(void) {
    return create_server();
}


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
