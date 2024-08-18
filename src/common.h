#include <arpa/inet.h>
#include <errno.h>
#include <netdb.h>
#include <pthread.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

// Custom Defines
#define BACKLOG 5        // max number of pending connections
#define ADDR "127.0.0.1" // IP address
#define PORT "3490"      // Port number
#define MAXDATASIZE 100
