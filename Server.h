#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <string.h>
#include <sys/wait.h>
#include <signal.h>
#include <netinet/in.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <pthread.h>
#include <time.h>
#include <sys/time.h>
#include "Util.h"
#include "Vote.h"
#define LISTENQUEUE 10
#define MAX_PARALLEL_CONNECTIONS 100
#define MAX_MSG_SIZE 100
#define TIMEOUT 10
#define MAXKEYS 1000

keyval_t keyVals[MAXKEYS];

void *serverThread (void *a);

int tcpServer( int port );

int responseServer(int sock,char*  msg);
