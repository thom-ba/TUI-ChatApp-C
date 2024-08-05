// (C) Thomas Baumeister, 2024
// For further information read the comment at the end of the file.

#ifndef CLIENT_H
#define CLIENT_H

// Standard Libraries
#include <string.h>
#include <stdio.h>
#include <stddef.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <unistd.h>
#include <netdb.h>
#include <pthread.h>

// Custom Defines
#define PORT "3490" // should be defined in server_h only
#define MAXDATASIZE 100


#endif // CLIENT_H
