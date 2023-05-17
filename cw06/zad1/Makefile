CC = gcc
CFLAGS = -Wall -ggdb

compile:
	make clean
	$(CC) $(CFLAGS) server.c -o server
	$(CC) $(CFLAGS) client.c -o client

clean:
	rm -rf client server server_records.txt

