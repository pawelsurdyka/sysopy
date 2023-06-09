#include <stdio.h>
#include <signal.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>

int main(int argc, char *argv[])
{
    if (argc != 2) {
        perror("Wrong number of arguments\n");
        exit(1);
    }
    
    if(strcmp(argv[1], "ignore") == 0)
    {
        fprintf(stdout, "Ignore in child process\n");
        raise(SIGUSR1);
    }
    else if(strcmp(argv[1], "mask") == 0)
    {
        fprintf(stdout, "Mask in child process\n");
        raise(SIGUSR1);
    }
    else if(strcmp(argv[1], "pending") == 0)
    {
        sigset_t pending_set;
        sigpending(&pending_set);
        if (sigismember(&pending_set, SIGUSR1))
        {
            fprintf(stdout, "Signal pending in child process. PID: %d\n", getpid());
        }
        else
        {
            fprintf(stdout, "Signal not pending in child process. PID: %d\n", getpid());
        }
    }
    

    return 0;
}
