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
#include "Vote.h"

#define LENGTH 1024
#define TIMEOUT 10
#define MAX_CON 1000

struct sockDes {
	int id;
        int sockfd;
	int connectionExists;
        struct sockaddr_in server_addr;

};



typedef struct hashMap{

        char keyValuePair[LENGTH];

        int count;

        int idx[MAX_CON];

}HashMap;



typedef struct hashTable{

        HashMap hashMap[MAX_CON];

}HashTable;



HashTable hashTable ;

keyval_t* keyVals_c;

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

int responseClient(int sock, char* msg); 

int selectServer();/*works on keyVals_c*/


