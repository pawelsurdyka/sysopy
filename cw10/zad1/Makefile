CC = gcc
CFLAGS = -Wall

all:
	make client
	make server

server: server.c
	$(CC) $(CFLAGS) server.c -o server -lpthread

client: client.c
	$(CC) $(CFLAGS) client.c -o client -lpthread

clean:
	rm -f server client
