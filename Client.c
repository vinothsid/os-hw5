#include "Client.h"

#include "Util.h"
#define LENGTH 1024
//#define client
//#define N 3
char msgG[LENGTH];
char msgType[100];
char keyG[LENGTH];
char valG[LENGTH];
int Nr;
int Nw;
int N;
int responsesG=0;
/*
int responseClient(int sock, char* msg) {
	char send_data[LENGTH],recv_data[LENGTH];
	
}
*/
int selectServer() {
        int maxIndex = -1;
        int i;
        int maxVno = -1;
        for(i=0;i<N;i++){
                if(keyVals_c[i].sock != -1 ){
                        //Check for the keyVal structure with the maximum version number
                        if(keyVals_c[i].vno > maxVno)
                                maxIndex = i;
                }
                else{
                        //printf("Skipped Index :%d\n",i);
                }
        }       
        
        // return the index value of the server with the highest version number
        return maxIndex;        
}

void* connectTo(void* sockfd) {
	char str[INET_ADDRSTRLEN];
	struct sockDes sock=*(struct sockDes *)sockfd;
	int bytes_received;
	int yes=1;
	char send_data[LENGTH],recv_data[LENGTH];
	inet_ntop(AF_INET,&(sock.server_addr.sin_addr),str,INET_ADDRSTRLEN);
//#ifdef client
	printf("ip addr is:%s connectionExists %d\n",str,sock.connectionExists);
//#endif
	if (setsockopt(sock.sockfd, SOL_SOCKET, SO_REUSEADDR, &yes,sizeof(int)) == -1) {
                perror("setsockopt");
                exit(1);
        }

	if (sock.connectionExists==0 && connect(sock.sockfd,(struct sockaddr *)&(sock.server_addr),
                    sizeof(struct sockaddr)) == -1)
	{
		int myid=sock.id;
		keyVals_c[myid].sock=-1;
		perror("Connect");
		pthread_exit(NULL);
	}
	send(sock.sockfd,msgG,strlen(msgG)+1,0);
	
	int myid=sock.id;
	memset(recv_data,0,LENGTH);/*zero data to check if something is received*/
	int rv=recvTimeout(sock.sockfd,recv_data,TIMEOUT,LENGTH);
//#ifdef client
	printf("sending data %s to %s at %d\n",msgG,str,ntohs(sock.server_addr.sin_port));
	printf("rv is %d:data received from client is %s\n",rv, recv_data);
//#endif
	if(rv==0 || rv==-1) {
		/*receive timed out*/
		perror("No Data From Server");
		keyVals_c[myid].sock=-1;
		pthread_exit(NULL);
	}
	/*atomically increment number of responses*/
	int resO=responsesG;
	int resN=resO+1;
	while(compare_and_swap(&responsesG,resN,resO)!=resO) {
		resO=responsesG;
		resN=resO+1;
	}
	
	sscanf(recv_data,"%d %s %s",&(keyVals_c[myid].vno),keyVals_c[myid].key,keyVals_c[myid].value);
	/*set sock to indicate server responded*/
	keyVals_c[myid].sock=sock.sockfd;
		
	if(strcmp(msgType,"PUT")==0) {
		char retMsg[25];
		sscanf(recv_data,"%s %*s",retMsg);
		if(strcmp(retMsg,"updatedone")==0) {
			char msgL[LENGTH+20];
			memset(msgL,0,LENGTH+20);
			strcpy(msgL,"releaselock");
			strcat(msgL," ");
			strcat(msgL,keyG);
			send(sock.sockfd,msgL,strlen(msgL),0);
		}
	}
	/*
	inet_ntop(AF_INET,&(sock.server_addr.sin_addr),str,INET_ADDRSTRLEN);
	printf("sending data %s to %s at %d\n",send_data,str,ntohs(sock.server_addr.sin_port));
	*/
}

int connectThread() {
	int sock;
	struct hostent* host;
	char str[INET_ADDRSTRLEN];
	struct sockaddr_in server_addr;
	FILE* server_loc;
	server_loc=fopen("server_loc.txt","r");
	printf("N is %d\n",N);
//	char addrArray[N][64]={"192.168.1.106", "192.168.1.106","192.168.1.106"};
//	int port[N]={5020,5010,5000};
	/*malloc addrArray and port*/
	char** addrArray=(char**)malloc(sizeof(char*)*N);
	int* port=(int*)malloc(sizeof(int)*N);
	int i;
	for(i=0;i<N;i++) {
		addrArray[i]=malloc(sizeof(char)*64);
		fscanf(server_loc,"%s %d",addrArray[i],&port[i]);
		printf("read addr : %s port : %d\n",addrArray[i],port[i]);
		
	}
	keyVals_c=(keyval_t*)malloc((sizeof(keyval_t))*N);
	//printf("%s\n",addrArray[0]);
	struct sockDes* sockfd=(struct sockDes *)malloc(sizeof(struct sockDes)*N);
	pthread_t* t=(pthread_t*)malloc(sizeof(pthread_t)*N);
	for(i=0;i<N;i++) {
#ifdef client
		printf("%d connect attempt\n",i);
#endif
		(sockfd+i)->id=i;
		if (((sockfd+i)->sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
			perror("Socket");
			exit(1);
		}
		(sockfd+i)->server_addr.sin_family=AF_INET;
		(sockfd+i)->server_addr.sin_port=htons(port[i]);
		host = gethostbyname(addrArray[i]);
		(sockfd+i)->server_addr.sin_addr = *((struct in_addr *)host->h_addr);
		bzero(&((sockfd+i)->server_addr.sin_zero),8);
		sockfd[i].connectionExists=0; 
		pthread_create(&t[i],NULL,connectTo,(void*)(&sockfd[i]));
	}
	for(i=0;i<N;i++) {
		pthread_join(t[i],NULL);
	}
#ifdef client
	for(i=0;i<N;i++) {
		char str[INET_ADDRSTRLEN];
		inet_ntop(AF_INET,&(sockfd[i].server_addr.sin_addr),str,INET_ADDRSTRLEN);
                int port_final=ntohs(sockfd[i].server_addr.sin_port);
		printf("sent data to %s at %d\n",str,port_final);
	}
#endif 
	if(strcmp(msgType,"GET")==0) {	
	/*if get was sent*/
		if(responsesG<Nr) {
		/*not enough votes for get*/
			exit(1);
		}
		int HVNO=selectServer();
		if(HVNO==-1) {
			perror("HVNO is -1");
			exit(1);
		}
		inet_ntop(AF_INET,&(sockfd[HVNO].server_addr.sin_addr),str,INET_ADDRSTRLEN);
		int port_final=ntohs(sockfd[HVNO].server_addr.sin_port);
		printf("highest version no at %s %d\n",str,port_final);	
		char* key_f=keyVals_c[HVNO].key;
		char* val_f=keyVals_c[HVNO].value;
		printf("Reading key/value: %s/%s\n",key_f,val_f);
	}
	else if(strcmp(msgType,"PUT")==0) {
	/*if UPDATE was sent*/
		if(responsesG<Nw) {
		/*not enough votes for put*/
			exit(1);
		}
		int HVNO=selectServer();
		if(HVNO==-1) {
			perror("Write failure");
			exit(1);
		}
		int new_vno=(keyVals_c[HVNO].vno)+1;
		
		printf("HVNO is : %d highest vno %d new_vno %d\n",HVNO,keyVals_c[HVNO].vno,new_vno);
		/*
		 * update all sockets which responded with vote
		 * i.e. sock in keyVals_c is not -1
		 */
		int i;
		/*create update msg*/
		memset(msgG,0,LENGTH);
		sprintf(msgG,"update %s %s %d",keyG,valG,new_vno);
		
		for(i=0;i<N;i++) {
			sockfd[i].connectionExists=1;
			if(keyVals_c[i].sock!=-1) {
				pthread_create(&t[i],NULL,connectTo,(void*)(&sockfd[i]));
			}
		}
		for(i=0;i<N;i++) {
			pthread_join(t[i],NULL);
		}
	}
}

void main(int argc, char* argv[]) {
	
	strcpy(msgType,argv[4]);
	strcpy(msgG,argv[4]);
	strcat(msgG," ");
	strcat(msgG,argv[5]);
	if(argc>6) {
		strcat(msgG," ");
		strcat(msgG,argv[6]);
		strcpy(keyG,argv[5]);
		strcpy(valG,argv[6]);
		//printf("msgG is %s argv[6] is %s\n",msgG,argv[6]);
		//strcpy(valG,argv[7]);
	}
	N=atoi(argv[3]);
	connectThread();
}
