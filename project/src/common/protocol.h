#ifndef PROTOCOL_H
#define PROTOCOL_H

#define DEFAULT_PORT 8080
#define MAX_USERNAME 31
#define MAX_MESSAGE 512
#define MAX_CLIENTS 32
#define PIPE_BUF_SIZE 1024

#define EVENT_LOGIN    1
#define EVENT_MESSAGE  2
#define EVENT_QUIT     3
#define EVENT_PRIVATE  4
#define EVENT_USERS    5

typedef struct {
    int type;
    int client_index;
    char username[MAX_USERNAME + 1];
    char target[MAX_USERNAME + 1];
    char message[MAX_MESSAGE + 1];
} server_event_t;

#endif
