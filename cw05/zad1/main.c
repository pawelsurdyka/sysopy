#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

void print_sorted_email(char *sort_by)
{
    FILE *file;
    char *command, line[LINE_MAX];
    printf("Printing emails ordered by %s\n", sort_by);
    if (strcmp(sort_by, "nadawca") == 0) {
        command = "echo | mail | tail -n +2 | head -n -1 | sort -k 2";
    } else if (strcmp(sort_by, "data") == 0) {
        command = "echo | mail | tail -n +2 | head -n -1 | tac";
    } else {
        perror("Wrong mode. Mode order should be sender or date\n");
        exit(1);
    }
    
    if ((file = popen(command, "r")) == NULL) {
        perror("popen error\n");
        exit(1);
    }
    while (fgets(line, LINE_MAX, file) != NULL) {
        printf("%s\n", line);
    }
    pclose(file);
}

void send_mail(char *address, char *subject, char *content)
{
    FILE *file;
    char command[LINE_MAX];
    snprintf(command, sizeof(command), "echo %s | mail -s %s %s", content, subject, address);
    file = popen(command, "r");

    if (file == NULL)
    {
        printf("popen erro\n");
        exit(-1);
    }
    
    printf("Sending email to: %s\nwith title: %s\nand content:\n%s", address, subject, content);
    pclose(file);
}

int main(int argc, char *argv[])
{
    if (argc != 2 && argc != 4)
    {
        printf("Wrong number of arguments :/\n");
        exit(-1);
    }
    if (argc == 2)
    {
        printf("Print emails sorted by [%s] \n", argv[1]);
        print_sorted_email(argv[1]);
    }
    else
    {
        printf("Sending email\n");
        send_mail(argv[1], argv[2], argv[3]);
    }
}
