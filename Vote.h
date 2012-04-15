#include "Util.h"
#define KEYSIZE 50
#define VALSIZE 50

/*
GET: get key
PUT: put key value
********
SENDVNO: vno key value //this ia a vote too
********
UPDATE: update vno key value
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

keyval_t* searchKey(char* key);

int updateKey(char* key, char* val, int vno);

//char* create_msg(char* msg,char* key,char* val,int vno);


