#include "Util.h"

int atomicIncr(int *var) {
	int tmp = *var;
	tmp = *var;

	while( compare_and_swap(var,tmp+1,tmp) != tmp ) {
		tmp = *var;
	}

	return 0;
}

int atomicDecr(int *var) {
        int tmp ;

	tmp= *var;
        while (compare_and_swap(var,tmp-1,tmp) != tmp ) {
		tmp = *var;
	}                   

        return 0;
}

                          
char *itoa(int num) {
        char *str;
        str = (char *)malloc(5);
        sprintf(str,"%d",num);
        return str;
}

int lll_lock(int* lll_val) {
        while(1) {
                while(*lll_val!=1) {
                }
                        if(compare_and_swap(lll_val,0,1))  {
                                return;
                        }//spin here
        }
}
/*
*unlocks the low level lock
*/
int lll_unlock(int* lll_val) {
        *lll_val=1;
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

