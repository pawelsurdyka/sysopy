CC=gcc
CFLAGS=-Wall

all: replace

replace: replace.c
	$(CC) $(CFLAGS) -o $@ $^

test: replace
	echo 'abcdef \nfedcba\ndefabc abcdef\n' > t1.txt
	./replace a x t1.txt t2.txt > pomiar_zad_1.txt

clean:
	rm -f replace *.txt
