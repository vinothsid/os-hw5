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
#include "myatomic.h"

char *itoa(int num);

int atomicIncr(int *var);

int atomicDecr(int *var);

int lll_lock(int* lll_val);

int lll_unlock(int* lll_val);
