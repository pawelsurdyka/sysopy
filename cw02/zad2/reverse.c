#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define BUFFER_SIZE 1024

void reverse_one(FILE* input_file,FILE* output_file){
    fseek(input_file, 0L, SEEK_END);
    long int file_size = ftell(input_file);
    fseek(input_file, 0L, SEEK_SET);

    char buffer1[1];
    for (long int i = file_size - 1; i >= 0; i -= 1) {
        fseek(input_file, i, SEEK_SET);
        fread(buffer1, 1,1, input_file);
        fwrite(buffer1, 1, 1, output_file);
    }
    
    fclose(input_file);
    fclose(output_file);
    
}

void swap(char* to_flip,int size){
    char temp;
    for(int i=0;i<size/2;i++){
        temp = to_flip[i];
        to_flip[i] = to_flip[size-i-1];
        to_flip[size-i-1] = temp;
    }
}


void reverse_buff(FILE* input_file,FILE* output_file){

    char buff[BUFFER_SIZE];
    long rest;
    
    fseek(input_file, 0L, SEEK_END);
    long int file_size = ftell(input_file);
    long int count = file_size/BUFFER_SIZE;


    for (long int i = count - 1; i >= 0; i -= 1){
        fread(buff,sizeof(char),BUFFER_SIZE,input_file);
        swap(buff,BUFFER_SIZE);
        fwrite(buff,sizeof(char),BUFFER_SIZE,output_file);
        fseek(input_file,-BUFFER_SIZE,SEEK_CUR);
    }
    
    rest = ftell(input_file);
    
    if(rest>0){
        fseek(input_file,0,SEEK_SET);
        fread(buff,sizeof(char),rest,input_file);
        swap(buff,rest);
        fwrite(buff,sizeof(char),rest,output_file);
    }
    fclose(input_file);
    fclose(output_file);
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        printf("Usage: %s input_file output_file\n", argv[0]);
        return 1;
    }

    FILE *input_file = fopen(argv[1], "rb");
    if (input_file == NULL) {
        printf("Error: Failed to open input file\n");
        return 1;
    }

    FILE *output_file = fopen(argv[2], "wb");
    if (output_file == NULL) {
        printf("Error: Failed to open output file\n");
        return 1;
    }

    clock_t one,many;
    
    one = clock();
    
    reverse_one(input_file,output_file);
    
    printf("One CPU time: %ld\n",clock()-one);
    
    many = clock();
    
    reverse_buff(input_file,output_file);
    
    printf("Many CPU time: %ld\n",clock()-many);

    return 0;
}
