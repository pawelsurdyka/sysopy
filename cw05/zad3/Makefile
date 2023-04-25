CC=gcc
CFLAGS=-Wall

all:
	make main
	make integral

main: main.c
	$(CC) $(CFLAGS) main.c -o main

integral: integral.c
	$(CC) $(CFLAGS) integral.c -o integral

tests:
	make all
	echo "tests" > report.txt
	./main 0.0000001 2  >> report.txt
	./main 0.0000001 5  >> report.txt
	./main 0.0000001 10 >> report.txt
	./main 0.00000001 2  >> report.txt
	./main 0.00000001 5  >> report.txt
	./main 0.00000001 10 >> report.txt

clean:
	rm -f main integral
