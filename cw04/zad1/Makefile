CC=gcc
CFLAGS=-Wall 

all: main 

main: main.c child
	$(CC) $(CFLAGS) main.c -o main

child: child.c
	$(CC) $(CFLAGS) child.c -o child

tests: main
	./main ignore fork
	./main handler fork
	./main mask fork
	./main pending fork
	./main ignore exec
	./main mask exec
	./main pending exec
clean:
	rm -f main child
