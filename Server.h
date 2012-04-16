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
#define MAX_MSG_SIZE 250
#define TIMEOUT 30
#define MAXKEYS 1000
#define MAX_KEY_SIZE 100
#define MAX_VAL_SIZE 100
#define MAX_MSGTYPE_SIZE 20

keyval_t keyVals[MAXKEYS];
int numKeys;
int numKeysLock;

void *serverThread (void *a);

int tcpServer( int port );

int responseServer(int sock,char*  msg);

keyval_t* searchKey(char* key);

int updateKey(char* key, char* val, int vno);

int getResponse(int sock,char *key) ;

int putResponse(int sock,char *key,char *val) ;

int updateResponse(int sock,char *key,char *val,int vno) ;

int releaselockResponse(int sock,char *key) ;
