#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <sys/wait.h>

void handler(int sig)
{
    printf("Correctly handled!\n");
    printf("Received signal %d. PID: %d, PPID: %d\n", sig, getpid(), getppid());
}

int main(int argc, char **argv)
{
    if(argc != 3)
    {
        printf("Wrong number of arguments!");
        exit(1);
    }
    
    if(strcmp(argv[2], "exec") == 0)
    {
        if (strcmp(argv[1], "ignore") == 0){
            printf("ignore\n");
            signal(SIGUSR1, SIG_IGN);
            printf("Raising signal\n");
            raise(SIGUSR1);
            
            if (execl("./child", "./child", argv[1], NULL) == -1) {
                printf("execl() error\n");
                exit(1);
            }
            
        }
        else if(strcmp(argv[1], "mask") == 0 || strcmp(argv[1], "pending") == 0){
            if (strcmp(argv[1], "mask") == 0)
            {
                printf("mask\n");
            }
            else
            {
                printf("pending\n");
            }
            
            sigset_t signal_set;
            sigemptyset(&signal_set);
            sigaddset(&signal_set, SIGUSR1);
            
            if (sigprocmask(SIG_BLOCK, &signal_set, NULL) < 0)
            {
                perror("Error while masking signal\n");
            }
            
            raise(SIGUSR1);
            
            sigpending(&signal_set);
            
            if (sigismember(&signal_set, SIGUSR1))
            {
                printf("Signal is pending. PID: %d\n", getpid());
            }
            else
            {
                printf("Signal is not pending. PID: %d\n", getpid());
            }
            
            printf("execl\n");
            
            if (execl("./child", "./child", argv[1], NULL) == -1) {
                printf("execl() error\n");
                exit(1);
            }

        }
        
    }
    else if(strcmp(argv[2], "fork") == 0){
        if(strcmp(argv[1], "ignore") == 0)
        {
            printf("ignore\n");
            signal(SIGUSR1, SIG_IGN);
            printf("Raising signal\n");
            raise(SIGUSR1);
        }
        if(strcmp(argv[1], "handler") == 0)
        {
            printf("handler\n");
            signal(SIGUSR1, handler);
            printf("Raising signal\n");
            raise(SIGUSR1);
        }
        if (strcmp(argv[1], "mask") == 0 || strcmp(argv[1], "pending") == 0) {
            if (strcmp(argv[1], "mask") == 0)
                printf("mask\n");
            else
                printf("pending\n");
                
            sigset_t signal_set;
            sigemptyset(&signal_set);
            sigaddset(&signal_set, SIGUSR1);
            
            if (sigprocmask(SIG_BLOCK, &signal_set, NULL) < 0)
            {
                perror("Error while masking signal\n");
            }
            
            printf("Raising signal\n");
            raise(SIGUSR1);

            sigpending(&signal_set);
            if (sigismember(&signal_set, SIGUSR1))
            {
                printf("Signal is pending. PID: %d\n", getpid());
            }
            else
            {
                printf("Signal is not pending. PID: %d\n", getpid());
            }
            
            
            if (fork() == 0) {
                if (strcmp(argv[1], "mask") == 0) {
                    printf("Raising signal\n");
                    raise(SIGUSR1);
                }
                sigpending(&signal_set);
                if (sigismember(&signal_set, SIGUSR1))
                    printf("Signal is pending in child process. PID: %d\n", getpid());
                else
                    printf("Signal is not pending in child process. PID: %d\n", getpid());
            }
            wait(NULL);
            
        }
    }

    

    return 0;
}

