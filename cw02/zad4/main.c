#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <ftw.h>

typedef struct stat stat_t;

long long sum = 0;

int p_size(const char* path,const stat_t* file_info,int t_flag){
    if(!S_ISDIR(file_info->st_mode)){
        printf("%15ld %15s\n",file_info->st_size,path);
        sum += file_info->st_size;
    }
    return 0;

}


int main(int argc,char* argv[]){
    if(argc < 2){
        fprintf(stderr,"Directory name is required\n");
	return 0;
    }
    if(argc > 2){
	fprintf(stderr,"To many arguments!\n");
	return 0;
    }
    ftw(argv[1],p_size,1);
    printf("Sumaryczny rozmiar: %lld\n",sum);
}
