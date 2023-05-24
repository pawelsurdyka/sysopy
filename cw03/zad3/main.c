#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <unistd.h>
#include <limits.h>
#include <errno.h>

void browsing_directory(char* path, char* target_string) {
    DIR* dir = opendir(path);
    if (dir == NULL) {
        perror("cannot open dir");
        exit(1);
    }

    struct dirent* entry;
    while ((entry = readdir(dir)) != NULL) {
        if (strcmp(entry->d_name, ".") == 0) {continue;}

        if (strcmp(entry->d_name, "..") == 0) {continue;}
        
        char file_path[PATH_MAX];
        snprintf(file_path, PATH_MAX, "%s/%s", path, entry->d_name);

        struct stat file_info;
        if (stat(file_path, &file_info) == -1) {
            perror("stat error");
            exit(1);
        }

        if (S_ISDIR(file_info.st_mode)) {
            pid_t pid = fork();
            if (pid == -1) {
                perror("error when calling fork");
                exit(1);
            }

            if (pid == 0) {
                browsing_directory(file_path, target_string);
                exit(0);
            }
        } 
        else  
        {
            FILE* file = fopen(file_path, "r");
            if (file == NULL) {
                perror("cannot open file");
                exit(1);
            }

            char buffer[PATH_MAX];

            fread(buffer,sizeof(char),PATH_MAX,file);
            if (strncmp(buffer, target_string, strlen(target_string)) == 0) {
		printf("%s %d\n", file_path,getpid());
	    } 

            if (fclose(file) != 0) {
                perror("cannot close file");
                exit(1);
            }
        }
    }

    if (closedir(dir) == -1) {
        perror("cannot close dir");
        exit(1);
    }
}

int main(int argc, char* argv[]) {
    if (argc != 3) {
        printf("Write directory path and string\n");
        exit(1);
    }
    
    if (strlen(argv[2]) >= 255) {
        printf("the input string cannot be longer than 255 bits\n");
        exit(1);
    } 

    browsing_directory(argv[1], argv[2]);
    while(wait(NULL)>0){}
    return 0;
}
