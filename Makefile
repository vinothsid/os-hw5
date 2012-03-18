all:
	gcc -g -c Cache.c -o Cache.o 
	gcc -g -Wall `pkg-config fuse --cflags --libs` fusexmp.c Cache.o -o fusexmp
clean:
	rm fusexmp Cache.o

