#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <fcntl.h> 
#include <unistd.h> 


void replace_f(char *from,char *to,FILE* input_file,FILE* output_file){
    fseek(input_file, 0L, SEEK_END);
    long int file_size = ftell(input_file);
    fseek(input_file, 0L, SEEK_SET);

    char buffer1[1];
    for (long int i = 0; i <= file_size - 1; i += 1) {
        fseek(input_file, i, SEEK_SET);
        fread(buffer1, 1,1, input_file);
        if (*buffer1 == *from) {
            *buffer1 = *to;
        }
        fwrite(buffer1, 1, 1, output_file);
    }

    fclose(input_file);
    fclose(output_file);

}

void replace(char* from, char* to, char* source_file, char* res_file){
    int input_file=open(source_file, O_RDONLY);
    int output_file=open(res_file, O_WRONLY | O_CREAT | O_TRUNC, 0600);
    
    char buff[1];
    
    while(read(input_file,buff,1)>0){
        if (*buff==*from){
            *buff = *to;
        }
        write(output_file,buff,1);
    }
    close(input_file);
    close(output_file);
}

int main(int argc, char *argv[]) {
    if (argc != 5) {
        printf("Niepoprawna liczba argumentow! Uzyj: %s <znak1> <znak2> <plik_wejsciowy> <plik_wyjsciowy>\n", argv[0]);
        return 1;
    }

    FILE *input_file = fopen(argv[3], "r");
    if (input_file == NULL) {
        printf("Nie mozna otworzyc pliku %s do odczytu!\n", argv[3]);
        return 1;
    }

    FILE *output_file = fopen(argv[4], "w");
    if (output_file == NULL) {
        printf("Nie mozna otworzyc pliku %s do zapisu!\n", argv[4]);
        return 1;
    }

    clock_t f_clk,clk;

    f_clk = clock();

    replace_f(argv[1],argv[2],input_file,output_file);

    printf("replace_f CPU time: %ld\n",clock()-f_clk);
    
    clk = clock();

    replace(argv[1],argv[2],argv[3],argv[4]);

    printf("replace CPU time: %ld\n",clock()-clk);

    return 0;
}
