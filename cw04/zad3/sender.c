#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>
#include <signal.h>
#include <stdlib.h>
#include <time.h>
#include <errno.h>

#define SIGNAL SIGUSR1

volatile bool confirmation_received = false;

void handler(int signo) {
    if (signo == SIGNAL) {
       confirmation_received = true;
    }
}

int sender(int catcher_pid, int* modes, int num_modes) {
    struct sigaction sender_action;
    sigemptyset(&sender_action.sa_mask);
    sender_action.sa_handler = handler;

    sigaction(SIGNAL, &sender_action, NULL);

    sigset_t mask, old_mask;
    sigemptyset(&mask);
    sigaddset(&mask, SIGNAL);

    for (int i = 0; i < num_modes; i++) {
        confirmation_received = false;
	
        union sigval val;
        val.sival_int = modes[i];
	sigqueue(catcher_pid, SIGNAL, val);

        sigprocmask(SIG_BLOCK, &mask, &old_mask);
        while (!confirmation_received) {
	    sigsuspend(&old_mask);
        }
        sigprocmask(SIG_SETMASK, &old_mask, NULL);
    }

    return 0;
}

int main(int argc, char *argv[]){
    int commands[argc-2];
    for(int i = 0;i<argc-2;i++){
        commands[i] = atoi(argv[i+2]);
    }

    int pid = atoi(argv[1]);

    sender(pid,commands,argc-2);

    return 0;
}
