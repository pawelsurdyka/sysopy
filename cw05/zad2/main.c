
#include<stdbool.h>
#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<math.h>
#include<sys/wait.h>
#include<time.h>


double f(double x) {
    return 4/(x*x+1);
} 

double calculate(double a, double b, double dx){
    double sum = 0;
    for(double i=a;i < b;i += dx){
        sum += f(i)*dx;
    }
    return sum;
}

double gather(int proc_num, double rec_len){
    int rec_num = (int)1.0/rec_len;
    pid_t fork_value;
    int** pipes = malloc(sizeof(int*)*proc_num); 
    int rec_per_proc = (int)rec_num/proc_num;
    double tmp;
    double sum;

    for(int i=0;i<proc_num;i++){
        pipes[i] = malloc(sizeof(int)*2);
        pipe(pipes[i]);
    }

    for(int i=0;i<proc_num;i++){
        fork_value = fork();
        if(fork_value==0){
            tmp = calculate(i*(rec_per_proc)*rec_len,(i+1)*(rec_per_proc)*rec_len,rec_len);
            write(pipes[i][1],&tmp,sizeof(double));
            exit(0);
        }
    }
    for(int i=0;i<proc_num;i++){
        read(pipes[i][0],&tmp,sizeof( double));
        sum += tmp;
    }
    free(pipes);
    return sum;
}

struct timespec timespec_diff(struct timespec start, struct timespec end) {
    struct timespec out;

    if ((end.tv_nsec-start.tv_nsec)<0) {
            out.tv_sec = end.tv_sec-start.tv_sec-1;
            out.tv_nsec = 1000000000+end.tv_nsec-start.tv_nsec;
    }
    else {
            out.tv_sec = end.tv_sec-start.tv_sec;
            out.tv_nsec = end.tv_nsec-start.tv_nsec;
    }
    return out;
}


int main(int argc, char *argv[])
{
    if (argc != 3) {
        fprintf(stderr, "Two arguments required \n");
        return 1;
    }
    int proc_num = atoi(argv[2]);
    double dx = atof(argv[1]);
    double val;
    struct timespec timespec_buff_start, timespec_buff_end;
    clock_gettime(CLOCK_REALTIME, &timespec_buff_start);

    val = gather(proc_num,dx);

    clock_gettime(CLOCK_REALTIME, &timespec_buff_end);
    struct timespec diff = timespec_diff(timespec_buff_start, timespec_buff_end);
    printf("RESULT: %lf\nN: %d\nDX: %.17f\nTIME: %lds %ldns\n\n", val, proc_num, dx, diff.tv_sec, diff.tv_nsec);
    return 0;
    
}
