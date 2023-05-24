#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Program takes one argument!\n");
        return 1;
    }

    int num_processes = atoi(argv[1]);

    for (int i = 0; i < num_processes; i++) {
        pid_t pid = fork();
        if (pid == -1) {
            perror("fork is equal -1");
            return 1;
        } else if (pid == 0) {
            printf("Parent PID: %d, Child PID: %d\n", getppid(), getpid());
            return 0;
        }
    }

    for (int i = 0; i < num_processes; i++) {
        wait(NULL);
    }

    printf("argv[1]: %s\n", argv[1]);

    return 0;
}
