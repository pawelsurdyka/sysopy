#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>

#define PIPE_PATH "/tmp/integral_queue"

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


int main(int argc, char** argv) {
    if (argc != 4) {
        perror("[INTEGRAL] wrong number of arguments (need 3)");
        return -1;
    }
    
    double dx =  atof(argv[1]);
    double a = atof(argv[2]);
    double b = atof(argv[3]);
    char buff[256] = "";

    double out = calculate(a, b, dx);
    size_t size = snprintf(buff, 256, "%lf\n", out);

    int pd = open(PIPE_PATH, O_WRONLY);
    write(pd, buff, size);
    close(pd);

    return 0;
}
