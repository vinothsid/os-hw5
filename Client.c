#include "Client.h"
#include "Util.h"
#define LENGTH 1024

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
	memset(send_data,0,LENGTH);
	memset(recv_data,0,LENGTH);
	strcpy(send_data,"test data");
	printf("sending data %s to %s at %d\n",send_data,str,ntohs(sock.server_addr.sin_port));
	send(sock.sockfd,send_data,strlen(send_data),0);
	recvTimeout(sock.sockfd,recv_data,timeout,LENGTH);
	printf("received data %s to %s at %d\n",recv_data,str,ntohs(sock.server_addr.sin_port));
		
}

int connectThread() {
	int sock;
	struct hostent* host;
	struct sockaddr_in server_addr;
	char addrArray[2][64]={"192.168.1.106", "192.168.1.106"};
	int port[2]={5000,5010};
	//printf("%s\n",addrArray[0]);
	int i;
	pthread_t t[2];
	if ((sock = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
		perror("Socket");
		exit(1);
	}
	for(i=0;i<2;i++) {
		printf("%d connect attempt\n",i);
		struct sockDes* sockfd=(struct sockDes *)malloc(sizeof(struct sockDes));
		sockfd->sockfd=sock;
		sockfd->server_addr.sin_family=AF_INET;
		sockfd->server_addr.sin_port=htons(port[i]);
		host = gethostbyname(addrArray[i]);
		sockfd->server_addr.sin_addr = *((struct in_addr *)host->h_addr);
		bzero(&(sockfd->server_addr.sin_zero),8); 
		pthread_create(&t[i],NULL,connectTo,(void*)sockfd);
	}
	for(i=0;i<2;i++) {
		pthread_join(t[i],NULL);
	}
	
}

