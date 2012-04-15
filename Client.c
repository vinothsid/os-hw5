#include "Client.h"
#include "Util.h"
#define LENGTH 1024
#define client

char msgG[100];
char msgType[100];
int Nr;
int Nw;
int N;
/*
int responseClient(int sock, char* msg) {
	char send_data[LENGTH],recv_data[LENGTH];
	
}
*/

void* connectTo(void* sockfd) {
	char str[INET_ADDRSTRLEN];
	struct sockDes sock=*(struct sockDes *)sockfd;
	int bytes_received;
	char send_data[LENGTH],recv_data[LENGTH];
	inet_ntop(AF_INET,&(sock.server_addr.sin_addr),str,INET_ADDRSTRLEN);
	printf("ip addr is:%s\n",str);
	if (connect(sock.sockfd,(struct sockaddr *)&(sock.server_addr),
                    sizeof(struct sockaddr)) == -1)
	{
		perror("Connect");
		pthread_exit(NULL);
	}
	send(sock.sockfd,msgG,strlen(msgG),0);
	
	int myid=sock.id;
	memset(recv_data,0,LENGTH);/*zero data to check if something is received*/
	recvTimeout(sock.sockfd,recv_data,TIMEOUT,LENGTH);
#ifdef client
	printf("data received from client is %s\n",recv_data);
#endif
	if(recv_data==0) {
		perror("No Data From Server");
		keyVals_c[myid].sock=-1;
		pthread_exit(NULL);
	}
	sscanf(recv_data,"%d %s %s",&(keyVals_c[myid].vno),keyVals_c[myid].key,keyVals_c[myid].value);
	/*set sock to indicate server responded*/
	keyVals_c[myid].sock=sock.sockfd;
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
	char addrArray[2][64]={"192.168.1.105", "192.168.1.106"};
	int port[2]={5000,5010};
	keyVals_c=(keyval_t*)malloc((sizeof(keyval_t))*N);
	//printf("%s\n",addrArray[0]);
	int i;
	struct sockDes* sockfd=(struct sockDes *)malloc(sizeof(struct sockDes)*N);
	pthread_t t[2];
	for(i=0;i<2;i++) {
		printf("%d connect attempt\n",i);
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
		pthread_create(&t[i],NULL,connectTo,(void*)(sockfd+i));
	}
	for(i=0;i<2;i++) {
		pthread_join(t[i],NULL);
	}
	if(strcmp(msgType,"GET")==0) {	
	/*if get was sent*/
		int HVNO=selectServer();
		inet_ntop(AF_INET,&(sockfd[HVNO].server_addr.sin_addr),str,INET_ADDRSTRLEN);
		int port_final=ntohs(sockfd[HVNO].server_addr.sin_port);
		printf("highest version no at %s %d\n",str,port_final);	
		char* key_f=keyVals_c[HVNO].key;
		char* val_f=keyVals_c[HVNO].value;
		printf("Reading key/value: %s/%s\n",key_f,val_f);
	}
}

void main(int argc, char* argv[]) {
	strcpy(msgType,argv[4]);
	strcpy(msgG,argv[4]);
	strcat(msgG," ");
	strcat(msgG,argv[5]);
	//printf("msg is %s\n",msgG);
	connectThread();
}
