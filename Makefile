all:
	gcc -g Util.c -c -o Util.o
	gcc -g Client.c Util.o -o client -lpthread
	gcc -g Server.c Util.o -o server -lpthread
client: Util
	gcc Client.c Util.o -o client -lpthread
Util:
	gcc Util.c -c -o Util.o
server: Util
	gcc Server.c Util.o -o server -lpthread
clean:
	rm Util.o client server
