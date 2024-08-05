#include <string.h>
#include <stddef.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <netdb.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

// Custom Defines
#define BACKLOG 5   // max number of pending connections
#define ADDR "127.0.0.1" // IP address
#define PORT "3490" // Port number
#define MAXDATASIZE 100 
