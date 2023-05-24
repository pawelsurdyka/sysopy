#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <stdbool.h>
#include <time.h>

#define SIGNAL SIGUSR1

volatile int procedure_count = 0;

void print_numbers() {
    for (int i = 1; i <= 100; i++)
    {
        printf("%d\n", i);
    }
}

void print_time() {
    time_t rawtime;
    struct tm* timeinfo;

    time(&rawtime);

    timeinfo = localtime(&rawtime);
    printf("Actual time:%s", asctime(timeinfo));
}

void print_request_count() {
    printf("Received %d requests\n", procedure_count);
}

void print_current_time() {
    while (true) {
        time_t rawtime;
        struct tm* timeinfo;

        time(&rawtime);

        timeinfo = localtime(&rawtime);
        printf("%s", asctime(timeinfo));
        sleep(1);
    }
}


void end() {
    printf("[CATCHER] Ending.\n");
    exit(0);
}

void handler(int signo, siginfo_t* siginfo, void* context) {
    if (signo == SIGNAL) {
        int state = siginfo->si_value.sival_int;
        printf("[CATCHER] Received signal %d.\n", state);

        procedure_count++;

        kill(siginfo->si_pid, SIGNAL);

        switch (state) {
            case 1:
                print_numbers();
                break;
            case 2:
                print_time();
                break;
            case 3:
                print_request_count();
                break;
            case 4:
                print_current_time();
                break;
            case 5:
		end();
            default:
                printf("[CATCHER] Invalid state %d.\n", state);
                break;
        }
    }
}

int main() {
    printf("[CATCHER] PID: %d\n", getpid());

    struct sigaction action;
    action.sa_sigaction = handler;
    sigemptyset(&action.sa_mask);
    action.sa_flags = SA_SIGINFO;

    sigaction(SIGNAL, &action, NULL);

    while (true) {
        sigsuspend(&action.sa_mask);
    }

    return 0;
}
