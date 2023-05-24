#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Write a path of catalog\n");
	return 1;
    }

    printf("Program name: %s\n", argv[0]);

    fflush(stdout);

    execl("/bin/ls", "ls", argv[1], NULL);

    return 0;
}
