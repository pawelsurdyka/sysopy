#define MESSAGE_LEN 256

typedef enum {
    PING,
    USERNAME_TAKEN,
    SERVER_FULL,
    DISCONNECT,
    GET,
    LIST,
    TO_ONE,
    TO_ALL,
    STOP,
} MessageType;

typedef struct {
    MessageType type;
    char nickname[MESSAGE_LEN];
    char text[MESSAGE_LEN];
} message;
