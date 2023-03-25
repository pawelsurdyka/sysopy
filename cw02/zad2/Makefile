CC=gcc
CFLAGS=-Wall 

all: reverse

reverse: reverse.c
	$(CC) $(CFLAGS) -o $@ $^

test: reverse
	echo "Hello world" > tekst1.txt
	./reverse tekst1.txt tekst2.txt > pomiar_zad_2.txt
	./reverse tekst2.txt tekst3.txt
	diff -s tekst1.txt tekst3.txt

clean:
	rm -f reverse *.txt
