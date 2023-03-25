#include <stdlib.h>
#include <stdio.h>
#include <dirent.h>
#include <sys/stat.h>


int main(void){
    DIR* directory = opendir(".");
    
    if(directory == NULL){
        fprintf(stderr, "Failed to open directory.\n");
        return EXIT_FAILURE;
    }
    
    long long sum = 0;
    
    struct dirent *dir_ent;
    struct stat file_stat;
    
    while((dir_ent = readdir(directory)) != NULL){
        char path[1024];
        const char *dirr = ".";
        dirr = ".";
        snprintf(path, sizeof(path), "%s/%s", dirr, dir_ent->d_name);
        
        stat(dir_ent->d_name,&file_stat);
        
        if(S_ISDIR(file_stat.st_mode)){
            continue;
        }
        if(S_ISLNK(file_stat.st_mode)){
            lstat(dir_ent->d_name,&file_stat);
        }
        
        sum += file_stat.st_size;
        printf("%15ld %15s\n",file_stat.st_size,dir_ent->d_name);

    }
    
    printf("Total size : %lld\n", sum);

    closedir(directory);

    
    return EXIT_SUCCESS;
}
