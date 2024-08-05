// (C) Thomas Baumeister, 2024
// For further information read the comment at the end of the file.

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

#define BACKLOG 5
#define ADDR "127.0.0.1"
#define PORT "3490"

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

int open_bind_socket() {
    int sock_fd;
    struct addrinfo hints, *res;

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    getaddrinfo(ADDR, PORT, &hints, &res);
    sock_fd = socket(res->ai_family, res->ai_socktype, res->ai_protocol); 

    set_sock_opt(sock_fd);

    bind(sock_fd, res->ai_addr, res->ai_addrlen);
    
    free(res);

    return sock_fd;
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
        
        total_sent += n;
        bytes_left -= n;
    }

    *len = total_sent;

    return n==-1?-1:0;
}

int main(){
    char client_ip[INET6_ADDRSTRLEN];
    char buf[256];

    int fd_count = 0;
    int fd_size = 5;

    struct pollfd *pfds = malloc(sizeof *pfds * fd_size);

    struct sockaddr_storage client_addr;
    socklen_t addr_len;

    int sock_fd;
    int new_fd;
    int listener;

    sock_fd = open_bind_socket();
    listener = get_listener(sock_fd);    
    ash_williams();

    pfds[0].fd = listener;
    pfds[0].events = POLLIN;
    fd_count = 1;

    printf("Server: Waiting for connections on: %s ...\n", ADDR);

    for(;;)
    {
        int poll_count = poll(pfds, fd_count, -1);
        if (poll_count == -1) {
            perror("poll");
            exit(1);
        }

        for (int i = 0; i < fd_count; i++) { // Loop through all file_descriptors
            if (pfds[i].revents & POLLIN) { // We got an event
                if (pfds[i].fd == listener) { // Is our listener
                    addr_len = sizeof client_addr;
                    if ( (new_fd = accept(listener, (struct sockaddr *)&client_addr, &addr_len) ) == -1) {
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
                    int nbytes = recv(pfds[i].fd, buf, sizeof buf, 0);
                    int sender_fd = pfds[i].fd;

                    if (nbytes == 0) { // Got an error or connection closed
                        if (nbytes <= 0) {
                            printf("Pollserver: socket %d hung up\n", sender_fd);
                        } else {
                            perror("recv");
                        }

                        close(sender_fd);
                        del_from_pfds(pfds, i, &fd_count);

                    } else { // We got data from client
                        printf("Server: received data: %s\n", buf);
                        for (int j = 0; j < fd_count; j++) {
                            int dst_fd = pfds[i].fd;
                            
                            if (dst_fd != listener && dst_fd != sender_fd) {
                                int len = strlen(buf);
                                if (send_all(dst_fd, buf, &len) == 1) {
                                    perror("send");
                                }
                            }
                        } 
                    }
                } // END handle data from poll
            } // END got an event
        } // END looping through file descriptors
    } // END for(;;)
}
