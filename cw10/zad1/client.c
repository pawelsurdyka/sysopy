#include <arpa/inet.h>
#include <errno.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/un.h>
#include <unistd.h>
#include "message.h"

int _;

int is_safe(int expr) {
  if (expr == -1) {
    printf("Failed");
    exit(EXIT_FAILURE);
  }
  return expr;
}

int unix_socket(char *path) {
    struct sockaddr_un addr;

    memset(&addr, 0, sizeof(addr));
    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path, path, sizeof addr.sun_path - 1);
    int sock = is_safe (socket(AF_UNIX, SOCK_STREAM, 0));
    is_safe (connect(sock, (struct sockaddr*) &addr, sizeof addr));

    return sock;
}


int weeb_socket(char *ipv4, int port) {
    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);

    if (inet_pton(AF_INET, ipv4, &addr.sin_addr) <= 0) {
        fprintf(stderr, "adress, %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }


    int sock = is_safe (socket(AF_INET, SOCK_STREAM, 0));
    is_safe (connect(sock, (struct sockaddr*) &addr, sizeof addr));

    return sock;
}


int sock;

void handleSIGINT(int signo) {
    message msg = { .type  =  DISCONNECT };
    _ = write(sock, &msg, sizeof msg);
    exit(0);
}

void handleSTDIN() {
    char buffer[512] = "";

    size_t x = read(STDIN_FILENO, &buffer, 512);
    buffer[x] = '\0';

    const char delim[] = " \t\n";

    int idx = 0;

    char *token;
    token = strtok(buffer, delim);

    MessageType type = -1;

    char other_nickname[MESSAGE_LEN] = {};
    char text[MESSAGE_LEN] = {};

    bool broke = false;

    if (!token) {
        return;
    }

    while (token) {
        switch(idx)
        {
          case 0:
              if (strcmp(token, "LIST") == 0) type = LIST;
              else if (strcmp(token, "2ALL") == 0) type = TO_ALL;
              else if (strcmp(token, "2ONE") == 0) type = TO_ONE;
              else if (strcmp(token, "STOP") == 0) type = STOP;
              else {
                  broke = true;
                  printf("Invalid command\n");
              }
              break;
          case 1:
              memcpy(text, token, strlen(token) * sizeof(char));
              text[strlen(token)] = 0;
              break;
          case 2:
              memcpy(other_nickname, token, strlen(token) * sizeof(char));
              other_nickname[strlen(token)] = 0;
              break;
          default:
              broke = true;
              break;
        }

        if (broke) break;

        idx++;
        token = strtok(NULL, delim);
    }

    if (broke) {
        return;
    }

    message msg;
    msg.type = type;
    memcpy(&msg.nickname, other_nickname, strlen(other_nickname) + 1);
    memcpy(&msg.text, text, strlen(text) + 1);
  
    _ = write(sock, &msg, sizeof msg);
}

void handleSOCK() {
    message msg;
    _ = read(sock, &msg, sizeof msg);

    switch (msg.type) {
        case USERNAME_TAKEN:
            printf("This name is taken\n");
            close(sock);
            exit(EXIT_FAILURE);

        case SERVER_FULL:
            printf("Server is full\n");
            close(sock);
            exit(EXIT_FAILURE);

        case PING:
            write(sock, &msg, sizeof msg);
            break;

        case STOP:
            printf("Stopping\n");
            close(sock);
            exit(EXIT_SUCCESS);

        case GET:
            printf("%s\n", msg.text);
            break;

        default:
            printf("Invalid message\n");
            break;
    }
}

int main(int argc, char *argv[]) {
    if (strcmp(argv[2], "web") == 0 && argc == 5) sock = weeb_socket(argv[3], atoi(argv[4]));
    else if (strcmp(argv[2], "unix") == 0 && argc == 4) sock = unix_socket(argv[3]);
    else {
        printf("Wrong arguments.\nUsage: [nick] [web] [port]\n");
        printf("Usage: [nick] [unix] [path]\n");
        exit(EXIT_FAILURE);
    }

    signal(SIGINT, handleSIGINT);

    char *nickname = argv[1];
    _ = write(sock, nickname, strlen(nickname));

    int epollFD = is_safe (epoll_create1(0));

    struct epoll_event stdinEvent = {
            .events = EPOLLIN | EPOLLPRI,
            .data = {.fd = STDIN_FILENO}
    };

    epoll_ctl(epollFD, EPOLL_CTL_ADD, STDIN_FILENO, &stdinEvent);

    struct epoll_event socketEvent = {
            .events = EPOLLIN | EPOLLPRI | EPOLLHUP,
            .data = {.fd = sock}
    };

    epoll_ctl(epollFD, EPOLL_CTL_ADD, sock, &socketEvent);

    struct epoll_event events[2];

    while (1) {
        int n = epoll_wait(epollFD, events, 2, 1);
        if (n == -1) {
            fprintf(stderr, "epoll_wait, %s\n", strerror(errno));
            exit(EXIT_FAILURE);
        }

        for (int i = 0; i < n; ++i) {
            if (events[i].data.fd == STDIN_FILENO) {
                handleSTDIN();
            } else {
                if (events[i].events & EPOLLHUP) {
                    printf("Disconnected\n");
                    exit(EXIT_SUCCESS);
                }

                handleSOCK();
            }
        }
    }
}
