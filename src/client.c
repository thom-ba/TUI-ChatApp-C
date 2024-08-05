// (C) Thomas Baumeister, 2024
// For further information read the comment at the end of the file.

#include "client.h"

// Description: todo

int create_client() {
    int sock_fd, numbytes;
    char buf[MAXDATASIZE]; 
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

    // handle user input  
    char input[1024];
    for(;;) {
        printf("Enter Message: ");
        if (scanf("%s", input) == EOF) {
            perror("scanf");

            exit(1);
        } else {
            send(sock_fd, input, strlen(input), 0);
        }
    }

    if ((numbytes = recv(sock_fd, buf, MAXDATASIZE-1, 0)) == -1) {
        perror("recv");
        exit(1);
    }

    buf[numbytes] = '\0';

    printf("Client: received: '%s'\n", buf);

    close(sock_fd);

    return 0;
}

int main(void) {
    return create_client();
}
