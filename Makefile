all:
	gcc Util.c -c -o Util.o
	gcc Client.c Util.o -o client -lpthread
	gcc Server.c Util.o -o server -lpthread
client:
	gcc Client.c Util.o -o client -lpthread
Util:
	gcc Util.c -c -o Util.o
server:
	gcc Server.c Util.o -o server -lpthread
