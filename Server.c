
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

#define LISTENQUEUE 10
#define MAX_PARALLEL_CONNECTIONS 100
#define MAX_MSG_SIZE 100
#define TIMEOUT 10

/* The following function is referred from Beej website: http://beej.us/guide/bgnet/output/html/singlepage/bgnet.html */
int recvtimeout(int s, char *buf, int len, int timeout)
{
    fd_set fds;
    int n;
    struct timeval tv;

    // set up the file descriptor set
    FD_ZERO(&fds);
    FD_SET(s, &fds);

    // set up the struct timeval for the timeout
    tv.tv_sec = timeout;
    tv.tv_usec = 0;

    // wait until timeout or data received
    n = select(s+1, &fds, NULL, NULL, &tv);
    if (n == 0) return -2; // timeout!
    if (n == -1) return -1; // error

    // data must be here, so do a normal recv()
    return recv(s, buf, len, 0);
}

void *serverThread (void *a){
        int *temp = (int *)a;
        int sockDesc = *temp;

	printf("Starting new thread\n");
	char msg[MAX_MSG_SIZE];
	int result ;

	while(1) {	
	result = recvtimeout(sockDesc,msg,MAX_MSG_SIZE,TIMEOUT); 


	if( result == -2 ) {
		printf("Timedout for socket %d\n",sockDesc);
	} else if (result>0) {
		printf("Received msg : %s\n",msg);
		memset(msg,0,MAX_MSG_SIZE);
		send(sockDesc,msg,result,0);
	} else {
		printf("Recv failed\n");
	}
	}
        close(sockDesc);
	free(a);
        return;

}

char *itoa(int num) {
        char *str;
        str = (char *)malloc(5);
        sprintf(str,"%d",num);
        return str;
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
