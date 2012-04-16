#include "Util.h"

char *itoa(int num) {
        char *str;
        str = (char *)malloc(5);
        sprintf(str,"%d",num);
        return str;
}

int recvTimeout(int sock,char* data,int timeout,int length) {
        fd_set readfds;
        struct timeval tv;
        FD_ZERO(&readfds);
        FD_SET(sock,&readfds);
        int n=sock+1;
        tv.tv_sec=timeout;
        tv.tv_usec=0;
        int rv=select(n,&readfds,NULL,NULL,&tv);
        //printf("return value from select is %d\n",rv);
        if(rv == -1) {
                perror("select");
		return rv;
        } else if(rv == 0) {
                printf("Timeout occured!\n");
		return rv;
        } else {
                return recv(sock,data,length,0);/*data must be available*/
        	;
	}
}

