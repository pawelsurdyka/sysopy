#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <sys/wait.h>
#include <time.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include<math.h>

#define PIPE_PATH "/tmp/integral_queue"


double gather(int proc_num, char* dx){
    char read_buff[2048] = "";
    char a[2048];
    char b[2048];

    double inv_n = 1.0/proc_num;
    
    mkfifo(PIPE_PATH, 0666);

    for (int i = 0; i < proc_num; i++)
        if (!fork()) {
            snprintf(a, 2048, "%lf", i*inv_n);
            snprintf(b, 2048, "%lf", (i+1)*inv_n);
            execl("./integral", "integral", dx, a, b, NULL);
        }

    double result = 0.0;
    
    int fifo = open(PIPE_PATH, O_RDONLY);
    int reading = 0;

    while(reading < proc_num) {
        size_t size = read(fifo, read_buff, 2048);
        read_buff[size] = 0;

        char delim[] = "\n";
        char* token;

        token = strtok(read_buff, delim);
        for (;token; token = strtok(NULL, delim)) {
            result += strtod(token, NULL);
            reading++;
        }
    }
    close(fifo);

    remove(PIPE_PATH);
    
    return result;
}


int main(int argc, char** argv) {
    double rec_len = strtod(argv[1],NULL);
    int proc_num = atoi(argv[2]);
    double result;
    
    struct timespec start;
    struct timespec end;
    double time;
    clock_gettime(CLOCK_REALTIME,&start);
    
    result = gather(proc_num,argv[1]);
    
    clock_gettime(CLOCK_REALTIME,&end);
    time = (end.tv_sec - start.tv_sec) + (double)(end.tv_nsec - start.tv_nsec) / 1000000000;

    printf("\nRESULT: %lf\nN: %d\nDX: %.17f\nTIME: %lf \n", result, proc_num, rec_len, time);

    return 0;
}

