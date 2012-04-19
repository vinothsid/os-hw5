#include "Server.h"
#define DEBUG 1

keyval_t* searchKey(char* key) {

	int i=0;
	for(i=0;i<numKeys;i++) {
		if(strcmp(key,keyVals[i].key) == 0) {
			return &keyVals[i];
		}
	}

	return NULL;
}

int updateKey(char* key, char* val, int vno) {
	keyval_t *keyToUpdate;
	keyToUpdate = searchKey(key);

	if(keyToUpdate  == NULL) {
		printf("Error: Key: %s not found in the Key-Value Table\n",key);
		return -1;
	} else if( keyToUpdate->lock ==  0 ) {
		printf("Error: The entry for Key:%s is not locked for updating\n",key);
		return -2;
	} else {
		strcpy(keyToUpdate->value,val);
		keyToUpdate->vno = vno;
		return 0;
	}

}

int getResponse(int sock,char *key) {
#ifdef DEBUG
	printf("GET : key:%s\n",key);
#endif
	char resMsg[MAX_MSG_SIZE];
	keyval_t *keyval = searchKey(key);
	if(keyval==NULL) {
#ifdef DEBUG
                printf("Error: Key: %s not found in the Key-Value Table\n",key);
#endif
                return -1;


	}

	lll_lock(&(keyval->condWaitLock));
	if( keyval->lock == 0 ) {
// incr getters of keyval
		keyval->numGetters++;

		lll_unlock(&(keyval->condWaitLock));

		if ( lier == 0 )
			sprintf(resMsg,"%d %s %s",keyval->vno,keyval->key,keyval->value);
		else {
			printf("LIER : lieing about value \n");
			sprintf(resMsg,"%d %s %slie",keyval->vno,keyval->key,keyval->value);
		}
#ifdef DEBUG
		printf("Sending msg: %s\n",resMsg);
#endif

		send(sock,resMsg,strlen(resMsg)+1,0);
//decr getters of keyval
	}
	else {
		lll_unlock(&(keyval->condWaitLock));
	}

	atomicDecr(&(keyval->numGetters));	
}

int initKeyValStruct( keyval_t *kv  ) {
	memset(kv->key,0,KEYSIZE);
	memset(kv->value,0,VALSIZE);
	kv->sock = -1;
	kv->vno =0 ;
	kv->lock = 0;
	kv->numGetters = 0;
	kv->condWaitLock = 1;

}
int putResponse(int sock,char *key,char *val) {
#ifdef DEBUG
	printf("PUT : key:%s value:%s \n",key,val);
#endif

	char resMsg[MAX_MSG_SIZE];
	char msg[MAX_MSG_SIZE];
	char msgType[MAX_MSGTYPE_SIZE];
	char keyNew[MAX_KEY_SIZE];
	char valNew[MAX_VAL_SIZE];
	keyval_t *keyval = searchKey(key);
	int vno;

	memset(resMsg,0,MAX_MSG_SIZE);
	memset(msg,0,MAX_MSG_SIZE);
/*
	check lock;
	check getters;
	reply;
	update;
	releaselock;
*/
        if(keyval == NULL) {
		lll_lock( &numKeysLock  );
			keyval = &keyVals[numKeys];
			numKeys++;
		lll_unlock( &numKeysLock );	
		
		initKeyValStruct(keyval);
		resMsg[0]='0';
		resMsg[1]= 0;
	} else {

		sprintf(resMsg,"%d %s %s",keyval->vno,keyval->key,keyval->value);
	}

	lll_lock(&(keyval->condWaitLock));
	if( keyval->lock == 0 && keyval->numGetters ==0 ) {
		keyval->lock=1;

		lll_unlock(&(keyval->condWaitLock));

#ifdef DEBUG
		printf("Sending msg: %s\n",resMsg);
#endif

		send(sock,resMsg,strlen(resMsg)+1,0);

		int res = recvTimeout(sock,msg,TIMEOUT,MAX_MSG_SIZE);

		if(res > 0) {

			sscanf(msg,"%s ",msgType);
#ifdef DEBUG
			printf("Received msg:%s\nReceived msg type: %s---\n",msg,msgType);
#endif
			
			if(strcmp(msgType,"update") == 0) {

				sscanf(msg,"%*s %s %s %d",keyNew,valNew,&vno);
				strcpy(keyval->key,keyNew);
				updateResponse(sock,keyNew,valNew,vno);

	
				memset(msg,0,MAX_MSG_SIZE);

				res = recvTimeout(sock,msg,TIMEOUT,MAX_MSG_SIZE);
				
				if(res > 0) {
					sscanf(msg,"%s ",msgType);
#ifdef DEBUG
					printf("Received msg type: %s old key:%s \n",msgType,keyNew);
#endif

					if(strcmp(msgType,"releaselock") == 0) {
					//	sscanf(msg,"%*s %s",keyNew);
						releaselockResponse(sock,keyNew);
					} else {
#ifdef DEBUG
						printf("Expected releaselock msg , but found %s\n",msgType);
#endif
						releaselockResponse(sock,key);
					}
			
				} else {
#ifdef DEBUG
					printf("Expected res value > 0 , but received : %d\n",res);
#endif
					releaselockResponse(sock,key);
					
				} 	
			} else {
#ifdef DEBUG
				printf("Expected update msg , but found %s\n",msgType);
#endif

				releaselockResponse(sock,key);
			}
		} else {
#ifdef DEBUG
			printf("PUT timedout hence releasing lock\n");
#endif				

			releaselockResponse(sock,key);
		}		

	} else {

		lll_unlock(&(keyval->condWaitLock));
	}

	return 0;


}

int updateResponse(int sock,char *key,char *val,int vno) {
#ifdef DEBUG
	printf("update : key:%s value:%s vno:%d \n",key,val,vno);
#endif
	char resMsg[MAX_MSG_SIZE];
	
	memset(resMsg,0,MAX_MSG_SIZE);

	if ( updateKey(key,val,vno) == 0 ) {
		sprintf(resMsg,"updatedone %s",key);
		send(sock,resMsg,strlen(resMsg)+1,0);
                return 0;
	} else {
#ifdef DEBUG
		printf("Updating key %s with value %s failed\n",key,val);
#endif

		return -1;
	}
}

int releaselockResponse(int sock,char *key) {
#ifdef DEBUG
	printf("releaselock : key:%s\n",key);
#endif

	keyval_t *kv = searchKey(key);

	if(kv != NULL ) {
		kv->lock = 0;
		return 0;
	} else {
		return -1;
	}
}

int responseServer(int sock,char*  msg) {
	char msgType[MAX_MSGTYPE_SIZE];
	char key[MAX_KEY_SIZE];
	char value[MAX_VAL_SIZE];
	int vno=-1;

	sscanf(msg,"%s ",msgType);
#ifdef DEBUG
	printf("Received msg type: %s---\n",msgType);
#endif
	sscanf(msg,"%*s %s",key);
	if(strcmp(msgType,"GET")==0) {

		getResponse(sock,key);

	} else if( strcmp(msgType,"PUT")==0 ) {

		sscanf(msg,"%*s %*s %s",value);
		putResponse(sock,key,value);

	} else if(strcmp(msgType,"update") == 0) {

		sscanf(msg,"%*s %*s %s %d",value,&vno);
		updateResponse(sock,key,value,vno);

	} else if(strcmp(msgType,"releaselock") == 0) {

		releaselockResponse(sock,key);

	} else {
		printf("Invalid msgtype:%s\n",msgType);
	}


}

void *serverThread (void *a){
        int *temp = (int *)a;
        int sockDesc = *temp;

	printf("Starting new thread\n");
	char msg[MAX_MSG_SIZE];
	memset(msg,0,MAX_MSG_SIZE);
	int numBytesRcvd ;

	numBytesRcvd = recv(sockDesc,msg,MAX_MSG_SIZE,0);

#ifdef DEBUG
	printf("Received msg:%s of size: %d\n",msg,numBytesRcvd);
#endif

	if(numBytesRcvd>0)
		responseServer(sockDesc,msg);
	else {
#ifdef DEBUG
		printf("Recv failed. numBytesRcvd is not positive\n");
#endif
	}
/*
	while(1) {	
	result = recvTimeout(sockDesc,msg,TIMEOUT,MAX_MSG_SIZE); 


	if( result == -2 ) {
		printf("Timedout for socket %d\n",sockDesc);
		break;
	} else if (result>0) {
		printf("Received msg : %s\n",msg);
		send(sockDesc,msg,result,0);
		memset(msg,0,MAX_MSG_SIZE);
	} else {
		printf("Recv failed\n");
		break;
	}
	}
*/
        close(sockDesc);
	free(a);
        return;

}



int tcpServer( int port )
{       pthread_t threadID[MAX_PARALLEL_CONNECTIONS];
//      struct serverParm *parmPtr;
        int *serverFd;
        int threadCount=0;
        char *msg1;
        struct msgToken* msgsock;
        int sockfd, new_fd, ret;  // listen on sock_fd, new connection on new_fd
        struct addrinfo hints, *servinfo, *p;/* hints are hints for get  addr info servinfo is server information and p is just used as a pointer*/
        struct sockaddr_storage their_addr; // connector's address information
        socklen_t sin_size;
        struct sigaction sa;
        int yes=1;
        char s[INET6_ADDRSTRLEN];
        int rv;
        memset(&hints, 0, sizeof hints);
        hints.ai_family = AF_UNSPEC;
        hints.ai_socktype = SOCK_STREAM;
        hints.ai_flags = AI_PASSIVE; // use my IP


        char *tempPort = itoa(port);
        if ((rv = getaddrinfo(NULL,tempPort, &hints, &servinfo)) != 0) {

                fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
                return 1;
        }

        sockfd = socket(servinfo->ai_family,servinfo->ai_socktype,servinfo->ai_protocol);


        if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes,sizeof(int)) == -1) {
                perror("setsockopt");
                exit(1);
        }

        if (bind(sockfd, servinfo->ai_addr, servinfo->ai_addrlen) == -1) {
                close(sockfd);
                perror("server: bind");

        }

       if (servinfo == NULL)  {
                fprintf(stderr, "server: failed to bind\n");
                return 2;
        }

        freeaddrinfo(servinfo); // all done with this structure

        if (listen(sockfd,LISTENQUEUE) == -1) {
                perror("listen");
                exit(1);
        }

/*
        sa.sa_handler = sigchld_handler; // reap all dead processes
        sigemptyset(&sa.sa_mask);
        sa.sa_flags = SA_RESTART;
        if (sigaction(SIGCHLD, &sa, NULL) == -1) {
                perror("sigaction");
                exit(1);
        }
*/
        printf("server: waiting for connections...\n");
        free(tempPort);

        while(1) {  // main accept() loop
                sin_size = sizeof their_addr;
                new_fd = accept(sockfd, (struct sockaddr *)&their_addr, &sin_size);
                if (new_fd == -1) {
                        perror("accept");
                        continue;
                }

                serverFd = (int *)malloc(sizeof(int));
                *serverFd = new_fd;

		if ( isReply() == 0 ) {
			close(new_fd);
			continue;
		}
                if(pthread_create(&threadID[threadCount],NULL,serverThread,(void *)serverFd)!=0){
                        printf("cannot create thread\n");
                }

                threadCount = (threadCount+1)%MAX_PARALLEL_CONNECTIONS;

        }
        return 0;
}

int isReply () {
        float x;
        //srand(time(NULL));
        x=rand() % 1024;
        x=x/1024.0;
        printf("Reply Probability number: %f expected value : %f \n",x,replyProbability);
        if (x < replyProbability) {
                return 1;
        } else {
                return 0;
        }
}

int main(int argc,char *argv[]) {

	if(argc != 4) {
		printf("Usage : server <port> <prob of replying back> <lier > \n");
		exit(1);
	}
	
	int serverPort = atoi(argv[1]);

	replyProbability = atof(argv[2]);
	lier = atoi(argv[3]);

	strcpy(keyVals[0].key,"key1");
	strcpy(keyVals[0].value,"value1");
	keyVals[0].lock=0;
	keyVals[0].vno=1;
	keyVals[0].condWaitLock = 1;
	numKeys=1;

	numKeysLock = 1;
	tcpServer(serverPort);

}
