#include "Server.h"

void *serverThread (void *a){
        int *temp = (int *)a;
        int sockDesc = *temp;

	printf("Starting new thread\n");
	char msg[MAX_MSG_SIZE];
	int result ;

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
                if(pthread_create(&threadID[threadCount],NULL,serverThread,(void *)serverFd)!=0){
                        printf("cannot create thread\n");
                }

                threadCount = (threadCount+1)%MAX_PARALLEL_CONNECTIONS;

        }
        return 0;
}

int main(int argc,char *argv[]) {
	int serverPort = atoi(argv[1]);
	tcpServer(serverPort);

}
