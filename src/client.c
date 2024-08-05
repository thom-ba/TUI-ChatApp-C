// (C) Thomas Baumeister, 2024
// For further information read the comment at the end of the file.

#include "client.h"

// Description: todo
void* handle_receive_message(void* arg)  {
    int sock_fd = *((int*) arg); 
    int numbytes;
    char buf[MAXDATASIZE];
    
    while(1) {
        if ( (numbytes = recv(sock_fd, buf, MAXDATASIZE-1, 0) ) == -1) {
            perror("recv");
            pthread_exit(NULL);
        }

        buf[numbytes] = '\0';

        printf("Client: received: '%s'\n", buf);
        fflush(stdout);

        memset(buf, 0, MAXDATASIZE);
    }
    return NULL;
}

void* handle_input(void *arg) {
    int sock_fd = *((int*) arg);
    char input[1024];

    while(1) {
        printf("Enter Message: ");

        if (fgets(input, sizeof(input), stdin) != NULL) {
            input[strcspn(input, "\n")] = '\0';

            if(send(sock_fd, input, strlen(input), 0) == -1) {
                perror("send");
                pthread_exit(NULL);
            }
        } else {
            perror("fgets");
            pthread_exit(NULL);

        }

        bzero(input, sizeof(input));
    }

    return NULL;
}


int create_client() {
    int sock_fd;
    struct addrinfo hints, *servinfo, *p;
    int rv;
    char s[INET6_ADDRSTRLEN];

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    if ((rv = getaddrinfo("localhost", PORT, &hints, &servinfo)) == -1) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        return -1;
    }

    for (p = servinfo; p != NULL; p = p->ai_next) {
        if ((sock_fd = socket(p->ai_family, p->ai_socktype,
                        p->ai_protocol)) == -1) {
            perror("Client: socket");
            continue;
        }

        if(connect(sock_fd, p->ai_addr, p->ai_addrlen) == -1) {
            close(sock_fd);
            perror("Client: connect");
            continue;
        }

        break;
    }

    if (p == NULL) {
        fprintf(stderr, "Client: failed to connect!\n");
        return 2;
    } 

    inet_ntop(p->ai_family, &((struct sockaddr_in *)p->ai_addr)->sin_addr, s, sizeof s); 
    printf("Client: connected to: %s\n", s);

    freeaddrinfo(servinfo);

    // receive message
    int receive_fd = sock_fd;
    pthread_t receive_thread; 
    int t_rm = pthread_create(&receive_thread, NULL,  &handle_receive_message, &receive_fd);


    // handle user input and sen
    int send_fd = sock_fd;
    pthread_t input_thread;
    int t_hi = pthread_create(&input_thread, NULL, &handle_input, (void*) &send_fd);

    pthread_join(receive_thread, NULL);
    pthread_join(input_thread, NULL); 

    // close the connection to the server

    return sock_fd;
}

int main(void) {
    int sock_fd = create_client();

    close(sock_fd);

    return EXIT_SUCCESS;
}
