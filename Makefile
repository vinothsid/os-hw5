all:
	gcc -Wall `pkg-config fuse --cflags --libs` fusexmp.c -o fusexmp
clean:
	rm fusexmp

