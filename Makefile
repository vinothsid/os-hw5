all:
	gcc Util.c -c -o Util.o
	gcc Client.c Util.o -o client -lpthread
	gcc Server.c Util.o -o server -lpthread
