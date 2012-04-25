#include "Client.h"

#include "Util.h"
//#define client
//#define N 3
int new_vnoG;
char msgG[LENGTH];
char msgType[100];
char keyG[LENGTH];
char valG[LENGTH];
int Nr;
int Nw;
int N;
int responsesG=0;
struct sockDes* sockfd;
/*
int responseClient(int sock, char* msg) {
	char send_data[LENGTH],recv_data[LENGTH];
	
}
*/



int selectServer() {

    int maxIndexCount = 0, i, j, /*maxVno = -1, */index=0;

	char tmp[LENGTH];

	int nrnwCheck=0;



	for(i=0;i<N;i++){

		hashTable.hashMap[i].count=0;

	}
	for(i=0;i<N;i++){
		if(keyVals_c[i].sock!=-1){

			nrnwCheck++;

		}
	}
	if(strcmp(msgType,"GET")==0){
		if(nrnwCheck<Nr){
			printf("Lesser Read Quorum\n");
			exit(0);
		}	
	}
	else{
		if(nrnwCheck<Nw){
			printf("Lesser Write Quorum\n");
			exit(0);
		}
	}
//	printf("Check Value:%d\n",nrnwCheck);
	int max=-1;
        for(i=0;i<N && msgType[0]=='G';i++){

                if(keyVals_c[i].sock != -1 && keyVals_c[i].vno>=0){
				int incrIndex=0;
				sprintf(tmp,"%s",keyVals_c[i].value);
				//strcpy(hashTable.hashMap[index].keyValuePair,tmp);
				if(keyVals_c[i].vno > max){
					index=0;
					hashTable.hashMap[index].count=0;
					max = keyVals_c[i].vno;
					strcpy(hashTable.hashMap[index].keyValuePair,tmp);
					hashTable.hashMap[index].idx[hashTable.hashMap[index].count]=i;
					hashTable.hashMap[index].count++;
				}



				else if(keyVals_c[i].vno == max){
					int xy=0;
					for(xy=0;xy<index+1;xy++) {
						if(strcmp(hashTable.hashMap[xy].keyValuePair,tmp)==0){
						       	hashTable.hashMap[xy].idx[hashTable.hashMap[xy].count]=i;
						       	hashTable.hashMap[xy].count++;
						 	incrIndex=1;
						}

					}
					if(incrIndex==0){
						hashTable.hashMap[index+1].count=0;
						strcpy(hashTable.hashMap[index+1].keyValuePair,tmp);
						hashTable.hashMap[index+1].idx[hashTable.hashMap[index+1].count]=i;
						hashTable.hashMap[index+1].count++;
						index++;
					}

				}

		       	}

        }       
	if(msgType[0]=='P') {
		index=0;
	}
	int maximum=-1;
	for(i=0;i<N && msgType[0]=='P';i++) {
		if(keyVals_c[i].sock != -1) {
			int incrIndex=0;
			int ij=0;
			for(ij=0;ij<index;ij++) {
				if(hashTable.hashMap[ij].ver_no==keyVals_c[i].vno) {
					hashTable.hashMap[ij].idx[hashTable.hashMap[ij].count]=i;
					hashTable.hashMap[ij].count++;
					incrIndex=1;
				}
			} 
			if(incrIndex==0) {		
				hashTable.hashMap[index].count=0;
				hashTable.hashMap[index].ver_no=keyVals_c[i].vno;
				hashTable.hashMap[index].idx[hashTable.hashMap[index].count]=i;
				hashTable.hashMap[index].count++;
				index++;
			}
		}
	}


	//printf("Index:%d||Max:%d \n",index+1,max);


/*
	for(i=0;i<index;i++){

		printf("Value:%d||Count:%d\n",hashTable.hashMap[i].ver_no, hashTable.hashMap[i].count);



	}	

*/	

	int retVal=0, retIndex=-1; int maxVno=-1;

	for(i=0;i<index+1;i++){
		if(hashTable.hashMap[i].count > retVal){
				retIndex = i;
				retVal = hashTable.hashMap[i].count;
				if(msgType[0]=='P') {
					maxVno=hashTable.hashMap[i].ver_no;
				}
		}	
	}
	for(i=0;i<index+1 && msgType[0]=='G';i++) {
		if(hashTable.hashMap[i].count < retVal ) {
			int x=0;
			for(x=0;x<hashTable.hashMap[i].count;x++) {
				int id=hashTable.hashMap[i].idx[x];
				char str[INET_ADDRSTRLEN];
				inet_ntop(AF_INET,&(sockfd[id].server_addr.sin_addr),str,INET_ADDRSTRLEN);
				int port_final=ntohs(sockfd[id].server_addr.sin_port);
				printf("server at %s:%d hostName %s lied\n",str,port_final,sockfd[id].hostName);
			}
		}
	}
	for(i=0;i<index+1 && msgType[0]=='P';i++) {
      		if(hashTable.hashMap[i].count!=retVal && hashTable.hashMap[i].ver_no>maxVno) {
			int x=0;
			for(x=0;x<hashTable.hashMap[i].count;x++) {
				int id=hashTable.hashMap[i].idx[x];
				char str[INET_ADDRSTRLEN];
				inet_ntop(AF_INET,&(sockfd[id].server_addr.sin_addr),str,INET_ADDRSTRLEN);
				int port_final=ntohs(sockfd[id].server_addr.sin_port);
				printf("server at %s:%d hostName %s lied\n",str,port_final,sockfd[id].hostName);
			}
		}
	}
	return hashTable.hashMap[retIndex].idx[0];

}

/*
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
*/
void* connectTo(void* sockfd) {
	int rnWait=rand()%250;/* a random amount of time to sleep so that servers get contacted in a random fashion*/
	usleep(rnWait);
	char str[INET_ADDRSTRLEN];
	struct sockDes sock=*(struct sockDes *)sockfd;
	int bytes_received;
	int yes=1;
	char send_data[LENGTH],recv_data[LENGTH];
	inet_ntop(AF_INET,&(sock.server_addr.sin_addr),str,INET_ADDRSTRLEN);
//#ifdef client
	//printf("ip addr is:%s connectionExists %d\n",str,sock.connectionExists);
//#endif
	if (setsockopt(sock.sockfd, SOL_SOCKET, SO_REUSEADDR, &yes,sizeof(int)) == -1) {
                perror("setsockopt");
                exit(1);
        }
	if(sock.connectionExists==1) {
		printf("updating Key : <%s> with Version <%d> Value <%s> at <%s><%d>\n",keyG,new_vnoG,valG,str,ntohs(sock.server_addr.sin_port));
	}
	if (sock.connectionExists==0 && connect(sock.sockfd,(struct sockaddr *)&(sock.server_addr),sizeof(struct sockaddr)) == -1)
	{
		int myid=sock.id;
		keyVals_c[myid].sock=-1;
		perror("Connect");
		close(sock.sockfd);
		pthread_exit(NULL);
	}
	send(sock.sockfd,msgG,strlen(msgG)+1,0);
	
	int myid=sock.id;
	memset(recv_data,0,LENGTH);/*zero data to check if something is received*/
	int rv=recvTimeout(sock.sockfd,recv_data,TIMEOUT,LENGTH);
//#ifdef client
	//printf("sending data %s to %s at %d\n",msgG,str,ntohs(sock.server_addr.sin_port));
//	printf("rv is %d:data received from client is %s\n",rv, recv_data);
//#endif
	if(rv==0 || rv==-1) {
		/*receive timed out*/
		perror("No Data From Server");
		keyVals_c[myid].sock=-1;
		close(sock.sockfd);
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
	if(sock.connectionExists==0) {
		sock.vote=1;
		printf("vote received from <%s><%d>\n",str,ntohs(sock.server_addr.sin_port));
	}			
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
	} else {
		close(sock.sockfd);
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
//	printf("N is %d\n",N);
//	char addrArray[N][64]={"192.168.1.106", "192.168.1.106","192.168.1.106"};
//	int port[N]={5020,5010,5000};
	/*malloc addrArray and port*/
	char** addrArray=(char**)malloc(sizeof(char*)*N);
	int* port=(int*)malloc(sizeof(int)*N);
	int i;
	sockfd=(struct sockDes *)malloc(sizeof(struct sockDes)*N);
	for(i=0;i<N;i++) {
		addrArray[i]=malloc(sizeof(char)*64);
		fscanf(server_loc,"%s %d",addrArray[i],&port[i]);
		strcpy(sockfd[i].hostName,addrArray[i]);
		sockfd[i].vote=0;
		//printf("read addr : %s port : %d\n",addrArray[i],port[i]);
		
	}
	keyVals_c=(keyval_t*)malloc((sizeof(keyval_t))*N);
	//printf("%s\n",addrArray[0]);
	pthread_t* t=(pthread_t*)malloc(sizeof(pthread_t)*N);
	for(i=0;i<N;i++) {
#ifdef client
		//printf("%d connect attempt\n",i);
#endif
		(sockfd+i)->id=i;
		if (((sockfd+i)->sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
			perror("Socket");
			exit(1);
		}
		(sockfd+i)->server_addr.sin_family=AF_INET;
		(sockfd+i)->server_addr.sin_port=htons(port[i]);
		if(host = gethostbyname(addrArray[i])==NULL) {
			perror("No such host");
		} else {
			(sockfd+i)->server_addr.sin_addr = *((struct in_addr *)host->h_addr);
		}
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
	for(i=0;i<N;i++) {
		if(sockfd[i].vote==1) {
			char str[INET_ADDRSTRLEN];	
			if(inet_ntop(AF_INET,&(sockfd[i].server_addr.sin_addr),str,INET_ADDRSTRLEN)!=NULL) {
				int port_final=ntohs(sockfd[i].server_addr.sin_port);
				printf("version no: <%d> at <%s> <%d>\n",keyVals_c[i].vno,str,port_final);
			}
		}
	}
	if(strcmp(msgType,"GET")==0) {	
	/*if get was sent*/
		//printf("total no of responses : %d\n",responsesG);
		if(responsesG<Nr) {
		/*not enough votes for get*/
		//	printf("total no of responses : %d\n",responsesG);
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
		//printf("total no of responses : %d\n",responsesG);
		if(responsesG<Nw) {
		/*not enough votes for put*/
		//	printf("total no of responses : %d\n",responsesG);
			exit(1);
		}
		int HVNO=selectServer();
		if(HVNO==-1) {
			perror("Write failure");
			exit(1);
		}
		//printf("HVNO is : %d\n",HVNO);
		new_vnoG=(keyVals_c[HVNO].vno)+1;
		//printf("HVNO is : %d highest vno %d new_vnoG %d\n",HVNO,keyVals_c[HVNO].vno,new_vnoG);
		
		/*
		 * update all sockets which responded with vote
		 * i.e. sock in keyVals_c is not -1
		 */
		int i;
		/*create update msg*/
		memset(msgG,0,LENGTH);
		sprintf(msgG,"update %s %s %d",keyG,valG,new_vnoG);
		
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
	}
	N=atoi(argv[1]);
	Nr=atoi(argv[2]);
	Nw=atoi(argv[3]);
	if(((Nr+Nw)> N) && (Nw >N/2.0) ) { 
		connectThread();
	} else {
		printf("Wrong conditions at command line\n");
	}
}
