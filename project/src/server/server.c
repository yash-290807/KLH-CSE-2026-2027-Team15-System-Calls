#include <arpa/inet.h>
#include <errno.h>
#include <netinet/in.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include "../common/protocol.h"

typedef struct {
    int active;
    int socket_fd;
    int pipe_fd;
    pid_t pid;
    char username[MAX_USERNAME + 1];
} client_t;

static client_t clients[MAX_CLIENTS];
static int server_fd = -1;
static volatile sig_atomic_t running = 1;

static void handle_sigint(int sig) {
    (void)sig;
    running = 0;
}

static void handle_sigchld(int sig) {
    (void)sig;
    while (waitpid(-1, NULL, WNOHANG) > 0) {
        /* Reap finished children. */
    }
}

static void die(const char *msg) {
    perror(msg);
    exit(EXIT_FAILURE);
}

static int send_all(int fd, const void *buf, size_t len) {
    const char *p = buf;
    while (len > 0) {
        ssize_t n = send(fd, p, len, MSG_NOSIGNAL);
        if (n < 0) {
            if (errno == EINTR) continue;
            return -1;
        }
        p += n;
        len -= (size_t)n;
    }
    return 0;
}

static int valid_username(const char *name) {
    size_t n = strlen(name);
    if (n == 0 || n > MAX_USERNAME) return 0;
    for (size_t i = 0; i < n; ++i) {
        unsigned char c = (unsigned char)name[i];
        if (!(c == '_' || c == '-' ||
              (c >= 'a' && c <= 'z') ||
              (c >= 'A' && c <= 'Z') ||
              (c >= '0' && c <= '9'))) {
            return 0;
        }
    }
    return 1;
}

static int username_exists(const char *name, int except_index) {
    for (int i = 0; i < MAX_CLIENTS; ++i) {
        if (i != except_index && clients[i].active &&
            strcmp(clients[i].username, name) == 0) {
            return 1;
        }
    }
    return 0;
}

static void send_text(int index, const char *text) {
    if (index < 0 || index >= MAX_CLIENTS || !clients[index].active) return;
    (void)send_all(clients[index].socket_fd, text, strlen(text));
}

static void broadcast_text(const char *text, int except_index) {
    for (int i = 0; i < MAX_CLIENTS; ++i) {
        if (clients[i].active && i != except_index) {
            send_text(i, text);
        }
    }
}

static void make_user_list(char *out, size_t out_size) {
    size_t used = 0;
    int count = 0;
    used += (size_t)snprintf(out + used, out_size - used,
                             "\n--- Online Users ---\n");
    for (int i = 0; i < MAX_CLIENTS && used < out_size; ++i) {
        if (clients[i].active) {
            used += (size_t)snprintf(out + used, out_size - used,
                                     "%d. %s\n", ++count, clients[i].username);
        }
    }
    if (used < out_size)
        snprintf(out + used, out_size - used, "--------------------\n");
}

static void remove_client(int index) {
    if (index < 0 || index >= MAX_CLIENTS || !clients[index].active) return;

    char notice[MAX_MESSAGE + 80];
    snprintf(notice, sizeof(notice), "[SERVER] %s left the chat.\n",
             clients[index].username);
    printf("%s", notice);
    broadcast_text(notice, index);

    close(clients[index].socket_fd);
    close(clients[index].pipe_fd);
    clients[index].active = 0;
    clients[index].socket_fd = -1;
    clients[index].pipe_fd = -1;
    clients[index].pid = -1;
    clients[index].username[0] = '\0';
}

static void child_loop(int client_fd, int pipe_write_fd, int index) {
    close(server_fd);

    char welcome[] =
        "========================================\n"
        "       MULTI-USER CHAT APPLICATION      \n"
        "========================================\n"
        "Enter username: ";
    if (send_all(client_fd, welcome, strlen(welcome)) < 0) _exit(1);

    char buf[MAX_MESSAGE + 128];
    ssize_t n = recv(client_fd, buf, sizeof(buf) - 1, 0);
    if (n <= 0) _exit(1);
    buf[n] = '\0';

    char username[MAX_USERNAME + 1];
    if (sscanf(buf, "%31s", username) != 1) _exit(1);

    server_event_t ev;
    memset(&ev, 0, sizeof(ev));
    ev.type = EVENT_LOGIN;
    ev.client_index = index;
    strncpy(ev.username, username, MAX_USERNAME);

    if (write(pipe_write_fd, &ev, sizeof(ev)) != (ssize_t)sizeof(ev)) _exit(1);

    const char *commands =
        "\nCommands:\n"
        "  /help\n"
        "  /users\n"
        "  /msg <user> <message>\n"
        "  /broadcast <message>\n"
        "  /quit\n\n";
    send_all(client_fd, commands, strlen(commands));

    for (;;) {
        n = recv(client_fd, buf, sizeof(buf) - 1, 0);
        if (n <= 0) break;
        buf[n] = '\0';

        /* Remove trailing CR/LF. */
        buf[strcspn(buf, "\r\n")] = '\0';
        if (buf[0] == '\0') continue;

        memset(&ev, 0, sizeof(ev));
        ev.client_index = index;
        strncpy(ev.username, username, MAX_USERNAME);

        if (strcmp(buf, "/quit") == 0) {
            ev.type = EVENT_QUIT;
            (void)write(pipe_write_fd, &ev, sizeof(ev));
            break;
        } else if (strcmp(buf, "/users") == 0) {
            ev.type = EVENT_USERS;
            (void)write(pipe_write_fd, &ev, sizeof(ev));
        } else if (strncmp(buf, "/msg ", 5) == 0) {
            ev.type = EVENT_PRIVATE;
            char *p = buf + 5;
            while (*p == ' ') ++p;
            char *space = strchr(p, ' ');
            if (!space) {
                send_all(client_fd, "[SERVER] Usage: /msg <user> <message>\n", 40);
                continue;
            }
            *space = '\0';
            strncpy(ev.target, p, MAX_USERNAME);
            strncpy(ev.message, space + 1, MAX_MESSAGE);
            (void)write(pipe_write_fd, &ev, sizeof(ev));
        } else if (strncmp(buf, "/broadcast ", 11) == 0) {
            ev.type = EVENT_MESSAGE;
            strncpy(ev.message, buf + 11, MAX_MESSAGE);
            (void)write(pipe_write_fd, &ev, sizeof(ev));
        } else if (strcmp(buf, "/help") == 0) {
            const char *help =
                "\nCommands:\n"
                "  /help                       Show commands\n"
                "  /users                      Show online users\n"
                "  /msg <user> <message>       Private message\n"
                "  /broadcast <message>        Broadcast message\n"
                "  /quit                       Leave chat\n"
                "  Any other text is broadcast.\n\n";
            send_all(client_fd, help, strlen(help));
        } else {
            ev.type = EVENT_MESSAGE;
            strncpy(ev.message, buf, MAX_MESSAGE);
            (void)write(pipe_write_fd, &ev, sizeof(ev));
        }
    }

    close(pipe_write_fd);
    close(client_fd);
    _exit(0);
}

int main(int argc, char **argv) {
    int port = DEFAULT_PORT;
    if (argc >= 2) {
        port = atoi(argv[1]);
        if (port < 1024 || port > 65535) {
            fprintf(stderr, "Port must be between 1024 and 65535.\n");
            return EXIT_FAILURE;
        }
    }

    memset(clients, 0, sizeof(clients));
    for (int i = 0; i < MAX_CLIENTS; ++i) {
        clients[i].socket_fd = -1;
        clients[i].pipe_fd = -1;
        clients[i].pid = -1;
    }

    struct sigaction sa_int = {0};
    sa_int.sa_handler = handle_sigint;
    sigemptyset(&sa_int.sa_mask);
    if (sigaction(SIGINT, &sa_int, NULL) == -1) die("sigaction SIGINT");

    struct sigaction sa_chld = {0};
    sa_chld.sa_handler = handle_sigchld;
    sigemptyset(&sa_chld.sa_mask);
    sa_chld.sa_flags = SA_RESTART | SA_NOCLDSTOP;
    if (sigaction(SIGCHLD, &sa_chld, NULL) == -1) die("sigaction SIGCHLD");

    signal(SIGPIPE, SIG_IGN);

    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == -1) die("socket");

    int opt = 1;
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1)
        die("setsockopt");

    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    addr.sin_port = htons((uint16_t)port);

    if (bind(server_fd, (struct sockaddr *)&addr, sizeof(addr)) == -1)
        die("bind");

    if (listen(server_fd, MAX_CLIENTS) == -1) die("listen");

    printf("========================================\n");
    printf("       MULTI-USER CHAT SERVER           \n");
    printf("========================================\n");
    printf("Listening on port: %d\n", port);
    printf("Server PID: %ld\n", (long)getpid());
    printf("Press Ctrl+C to stop.\n\n");
    fflush(stdout);

    while (running) {
        fd_set readfds;
        FD_ZERO(&readfds);
        FD_SET(server_fd, &readfds);
        int maxfd = server_fd;

        for (int i = 0; i < MAX_CLIENTS; ++i) {
            if (clients[i].active && clients[i].pipe_fd >= 0) {
                FD_SET(clients[i].pipe_fd, &readfds);
                if (clients[i].pipe_fd > maxfd) maxfd = clients[i].pipe_fd;
            }
        }

        int ready = select(maxfd + 1, &readfds, NULL, NULL, NULL);
        if (ready < 0) {
            if (errno == EINTR) continue;
            perror("select");
            break;
        }

        /* Handle events coming from child processes. */
        for (int i = 0; i < MAX_CLIENTS; ++i) {
            if (!clients[i].active || clients[i].pipe_fd < 0 ||
                !FD_ISSET(clients[i].pipe_fd, &readfds)) {
                continue;
            }

            server_event_t ev;
            ssize_t n = read(clients[i].pipe_fd, &ev, sizeof(ev));

            if (n == 0) {
                /* Child closed its pipe: unexpected client/process exit. */
                if (clients[i].active) remove_client(i);
                continue;
            }

            if (n != (ssize_t)sizeof(ev)) {
                if (n < 0 && errno == EINTR) continue;
                fprintf(stderr, "[WARN] Incomplete event from client slot %d.\n", i);
                continue;
            }

            if (ev.type == EVENT_LOGIN) {
                if (!valid_username(ev.username)) {
                    send_text(i, "LOGIN_FAIL Invalid username. Use letters, numbers, _ or -.\n");
                    remove_client(i);
                    continue;
                }
                if (username_exists(ev.username, i)) {
                    send_text(i, "LOGIN_FAIL Username already in use.\n");
                    remove_client(i);
                    continue;
                }

                strncpy(clients[i].username, ev.username, MAX_USERNAME);
                clients[i].username[MAX_USERNAME] = '\0';

                char notice[MAX_MESSAGE + 80];
                snprintf(notice, sizeof(notice),
                         "[SERVER] %s joined the chat.\n", clients[i].username);
                printf("%s", notice);
                fflush(stdout);
                broadcast_text(notice, i);
            } else if (ev.type == EVENT_MESSAGE) {
                char out[MAX_MESSAGE + MAX_USERNAME + 20];
                snprintf(out, sizeof(out), "%s: %s\n",
                         clients[i].username, ev.message);
                printf("%s", out);
                fflush(stdout);
                broadcast_text(out, -1);
            } else if (ev.type == EVENT_PRIVATE) {
                int target = -1;
                for (int j = 0; j < MAX_CLIENTS; ++j) {
                    if (clients[j].active &&
                        strcmp(clients[j].username, ev.target) == 0) {
                        target = j;
                        break;
                    }
                }

                if (target < 0) {
                    send_text(i, "[SERVER] User not found.\n");
                } else {
                    char out[MAX_MESSAGE + MAX_USERNAME * 2 + 30];
                    snprintf(out, sizeof(out), "[PRIVATE] %s -> %s: %s\n",
                             clients[i].username, ev.target, ev.message);
                    send_text(target, out);
                    if (target != i) send_text(i, out);
                }
            } else if (ev.type == EVENT_USERS) {
                char list[MAX_CLIENTS * (MAX_USERNAME + 10) + 64];
                make_user_list(list, sizeof(list));
                send_text(i, list);
            } else if (ev.type == EVENT_QUIT) {
                remove_client(i);
            }
        }

        /* Accept new clients without blocking message processing. */
        if (running && FD_ISSET(server_fd, &readfds)) {
            struct sockaddr_in client_addr;
            socklen_t client_len = sizeof(client_addr);
            int client_fd = accept(server_fd,
                                   (struct sockaddr *)&client_addr, &client_len);

            if (client_fd < 0) {
                if (errno != EINTR) perror("accept");
                continue;
            }

            int slot = -1;
            for (int i = 0; i < MAX_CLIENTS; ++i) {
                if (!clients[i].active) {
                    slot = i;
                    break;
                }
            }

            if (slot == -1) {
                const char *full = "[SERVER] Server is full. Try again later.\n";
                send_all(client_fd, full, strlen(full));
                close(client_fd);
                continue;
            }

            int pipefd[2];
            if (pipe(pipefd) == -1) {
                perror("pipe");
                close(client_fd);
                continue;
            }

            pid_t pid = fork();
            if (pid < 0) {
                perror("fork");
                close(pipefd[0]);
                close(pipefd[1]);
                close(client_fd);
                continue;
            }

            if (pid == 0) {
                close(pipefd[0]);
                child_loop(client_fd, pipefd[1], slot);
            }

            close(pipefd[1]);
            clients[slot].active = 1;
            clients[slot].socket_fd = client_fd;
            clients[slot].pipe_fd = pipefd[0];
            clients[slot].pid = pid;
            clients[slot].username[0] = '\0';

            printf("[INFO] Client connected: slot=%d pid=%ld\n",
                   slot, (long)pid);
            fflush(stdout);
        }
    }

    printf("\n[INFO] Shutting down server...\n");
    for (int i = 0; i < MAX_CLIENTS; ++i) {
        if (clients[i].active) {
            const char *msg = "\n[SERVER] Server is shutting down.\n";
            send_text(i, msg);
            kill(clients[i].pid, SIGTERM);
            close(clients[i].socket_fd);
            close(clients[i].pipe_fd);
        }
    }

    close(server_fd);
    while (waitpid(-1, NULL, 0) > 0) {}
    printf("[INFO] Server stopped cleanly.\n");
    return 0;
}
