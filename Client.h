#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <pthread.h>

#define TIMEOUT 10
struct sockDes {
        int sockfd;
        struct sockaddr_in server_addr;
};

/*
*Thread function which calls connect system call
*also receives data using the recvTimeout function
*/
void* connectTo(void* sockfd);

/*
*This function reads the locations of servers
*and spawns threads which create tcp connections
*with the servers
*internally levarages the connectTo function
*/
int connectThread();

