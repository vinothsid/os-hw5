#include "Util.h"
#include <stdio.h>
#define KEYSIZE 50
#define VALSIZE 50

/*
GET: get key
PUT: put key value
********
SENDVNO: vno key value //this ia a vote too
********
UPDATE: update key value vno
********
UPDATEDONE: updatedone key
********
RELEASELOCK: releaselock key
********
*/
typedef struct keyVAL {
	char key[KEYSIZE];
	char value[VALSIZE];
	int sock;
	int vno;
	int lock;
} keyval_t;



