#include <pthread.h>
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

#define MAX_CONN 16
#define PING_INTERVAL 20

int is_safe(int expr) {
  if (expr == -1) {
    printf("Failed");
    exit(EXIT_FAILURE);
  }
  return expr;
}

typedef enum {
    empty = 0, init, ready
} ClientStateEnum;

typedef struct {
    int fd;
    ClientStateEnum state;
    char nickname[MESSAGE_LEN];
    char symbol;
    bool responding;
} ClientStruct;

typedef enum {
    socket_event, client_event
} EventTypeEnum;

typedef union {
    ClientStruct *client;
    int socket;
} PayloadUnion;

typedef struct {
    EventTypeEnum type;
    PayloadUnion payload;
} EventDataStruct;


pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

int epollFD;

ClientStruct clients[MAX_CONN];

void deleteClient(ClientStruct *client) {
    printf("Deleting %s\n", client->nickname);
    client->state = empty;
    client->nickname[0] = 0;
    epoll_ctl(epollFD, EPOLL_CTL_DEL, client->fd, NULL);
    close(client->fd);
}

void sendMessage(ClientStruct *client, MessageType type, char text[MESSAGE_LEN]) {
    message msg;
    msg.type = type;
    memcpy(&msg.text, text, MESSAGE_LEN * sizeof(char));
    write(client->fd, &msg, sizeof(msg));
}

void onClientMessage(ClientStruct *client) {
    if (client->state == init) {
        int nick_size = read(client->fd, client->nickname, sizeof client->nickname - 1);
        int clientIndex = (int) (client - clients);

        pthread_mutex_lock(&mutex);

        int res = -1;
        for (int i = 0; i < MAX_CONN; i++) {
            if (i != clientIndex && strncmp(client->nickname, clients[i].nickname, sizeof client->nickname) == 0) {
                res = i;
                break;
            }
        }

        if (res == -1) {
            client->nickname[nick_size] = '\0';
            client->state = ready;

            printf("New client %s\n", client->nickname);
        } else {
            message msg = {.type = USERNAME_TAKEN};
            write(client->fd, &msg, sizeof msg);

            printf("Nickname %s is occupied\n", client->nickname);

            strcpy(client->nickname, "uwu");
            deleteClient(client);
        }
        pthread_mutex_unlock(&mutex);
    } else {
        message msg;
        read(client->fd, &msg, sizeof msg);

        printf("Got message %i %s\n", (int) msg.type, msg.text);

        if (msg.type == PING) {
            pthread_mutex_lock(&mutex);
            printf("pong %s\n", client->nickname);
            client->responding = true;
            pthread_mutex_unlock(&mutex);
        } else if (msg.type == DISCONNECT || msg.type == STOP) {
            pthread_mutex_lock(&mutex);
            deleteClient(client);
            pthread_mutex_unlock(&mutex);
        } else if (msg.type == TO_ALL) {
            char out[256] = "";
            strcat(out, client->nickname);
            strcat(out, ": ");
            strcat(out, msg.text);
            for (int i = 0; i < MAX_CONN; i++) {
                if (clients[i].state != empty) {
                    sendMessage(clients + i, GET, out);
                }
            }
        } else if (msg.type == LIST) {
            for (int i = 0; i < MAX_CONN; i++) {
                if (clients[i].state != empty) {
                    sendMessage(client, GET, clients[i].nickname);
                }
            }
        } else if (msg.type == TO_ONE) {
            char out[256] = "";
            strcat(out, client->nickname);
            strcat(out, ": ");
            strcat(out, msg.text);
            for (int i = 0; i < MAX_CONN; i++) {
                if (clients[i].state != empty) {
                    if (strcmp(clients[i].nickname, msg.nickname) == 0) {
                        sendMessage(clients + i, GET, out);
                    }
                }
            }
        }
    }
}


void initSocket(int socket, void *addr, int addr_size) {
    is_safe (bind(socket, (struct sockaddr*) addr, addr_size));
    is_safe (listen(socket, MAX_CONN));
    struct epoll_event event = {.events = EPOLLIN | EPOLLPRI};

    EventDataStruct *event_data = event.data.ptr = malloc(sizeof *event_data);
    event_data->type = socket_event;
    event_data->payload.socket = socket;
    epoll_ctl(epollFD, EPOLL_CTL_ADD, socket, &event);
}


ClientStruct *newClient(int client_fd) {
    pthread_mutex_lock(&mutex);
    int i = -1;

    for (int j = 0; j < MAX_CONN; j++) {
        if (clients[j].state == empty) {
            i = j;
            break;
        }
    }

    if (i == -1) return NULL;

    ClientStruct *client = &clients[i];
    client->fd = client_fd;
    client->state = init;
    client->responding = true;
    pthread_mutex_unlock(&mutex);
    return client;
}


void *ping() {
    message msg = {.type = PING};
    while (1) {
        sleep(PING_INTERVAL);
        pthread_mutex_lock(&mutex);
        printf("Pinging clients\n");

        for (int i = 0; i < MAX_CONN; i++) {
            if (clients[i].state != empty) {
                if (clients[i].responding) {
                    clients[i].responding = false;
                    write(clients[i].fd, &msg, sizeof msg);
                } else {
                    deleteClient(&clients[i]);
                }
            }
        }

        pthread_mutex_unlock(&mutex);
    }
}


int main(int argc, char **argv) {
    if (argc != 3) {
        printf("Wrong arguments. Usage: [port] [path]\n");
        exit(EXIT_FAILURE);
    }

    int port = atoi(argv[1]);
    char *socketPath = argv[2];

    epollFD = is_safe (epoll_create1(0));

    struct sockaddr_un localAddress = {.sun_family = AF_UNIX};
    strncpy(localAddress.sun_path, socketPath, sizeof localAddress.sun_path);

    struct sockaddr_in webAddress = {
            .sin_family = AF_INET,
            .sin_port = htons(port),
            .sin_addr = {.s_addr = htonl(INADDR_ANY)},
    };

    unlink(socketPath);
    int localSock = is_safe(socket(AF_UNIX, SOCK_STREAM, 0));
    initSocket(localSock, &localAddress, sizeof localAddress);

    int webSock = is_safe(socket(AF_INET, SOCK_STREAM, 0));
    initSocket(webSock, &webAddress, sizeof webAddress);

    pthread_t pingThread;
    pthread_create(&pingThread, NULL, ping, NULL);

    printf("Server is listening on: %d, '%s'\n", port, socketPath);

    struct epoll_event events[10];

    while (1) {
        int n = epoll_wait(epollFD, events, 10, -1);
        if (n == -1) {
            fprintf(stderr, "epoll_wait, %s\n", strerror(errno));
            exit(EXIT_FAILURE);
        }

        for (int i = 0; i < n; i++) {
            EventDataStruct *data = events[i].data.ptr;

            if (data->type == socket_event) {
                int clientFD = accept(data->payload.socket, NULL, NULL);
                ClientStruct *client = newClient(clientFD);

                if (!client) {
                    printf("Server is at capacity\n");
                    message msg = {.type = SERVER_FULL};
                    write(clientFD, &msg, sizeof msg);
                    close(clientFD);
                    continue;
                }

                EventDataStruct *eData = malloc(sizeof(EventDataStruct));
                eData->type = client_event;
                eData->payload.client = client;

                struct epoll_event event = {
                        .events = EPOLLIN | EPOLLET | EPOLLHUP,
                        .data = {eData}
                };

                if (epoll_ctl(epollFD, EPOLL_CTL_ADD, clientFD, &event) == -1) {
                    fprintf(stderr, "epoll_ctl, %s\n", strerror(errno));
                    exit(EXIT_FAILURE);
                }
            } else if (data->type == client_event) {
                if (events[i].events & EPOLLHUP) {
                    pthread_mutex_lock(&mutex);
                    deleteClient(data->payload.client);
                    pthread_mutex_unlock(&mutex);
                } else {
                    onClientMessage(data->payload.client);
                }
            }
        }
    }
}
