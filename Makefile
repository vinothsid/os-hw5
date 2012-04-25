EXTRA_CFLAGS = -lrt -w -lpthread
EXTRA_CFLAGS_CUDA = -arch=sm_13

CUDA_CC = /usr/local/cuda/bin/nvcc

all:
	gcc -g Util.c -c -o Util.o
	gcc -g Client.c Util.o -o client -lpthread
	gcc -g Server.c Util.o -o server -lpthread
	$(CUDA_CC) $(EXTRA_CFLAGS) $(EXTRA_CFLAGS_CUDA) -o a3 p3.cu
client: Util
	gcc Client.c Util.o -o client -lpthread
Util:
	gcc Util.c -c -o Util.o
server: Util
	gcc Server.c Util.o -o server -lpthread
a3: p3.cu
	$(CUDA_CC) $(EXTRA_CFLAGS) $(EXTRA_CFLAGS_CUDA) -o $@ $^
clean:
	rm Util.o client server a3 *~
