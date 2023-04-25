CC=gcc
CFLAGS=-Wall

all: main

main: main.c
	$(CC) $(CFLAGS) main.c -o main

tests:
	make all
	echo "tests" > report.txt
	./main 0.000000001 1  >> report.txt
	./main 0.000000001 5  >> report.txt
	./main 0.000000001 10 >> report.txt
	./main 0.000000002 1  >> report.txt
	./main 0.000000002 5  >> report.txt
	./main 0.000000002 10 >> report.txt
	./main 0.000000005 1  >> report.txt
	./main 0.000000005 5  >> report.txt
	./main 0.000000005 10 >> report.txt

clean:
	rm -f main

