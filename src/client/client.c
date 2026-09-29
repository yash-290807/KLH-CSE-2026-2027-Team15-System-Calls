#include <arpa/inet.h>
#include <errno.h>
#include <netinet/in.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <unistd.h>

#include "../common/protocol.h"

static volatile sig_atomic_t running = 1;

static void handle_sigint(int sig) {
    (void)sig;
    running = 0;
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

int main(int argc, char **argv) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <server_ip> <port>\n", argv[0]);
        return EXIT_FAILURE;
    }

    int port = atoi(argv[2]);
    if (port < 1024 || port > 65535) {
        fprintf(stderr, "Invalid port.\n");
        return EXIT_FAILURE;
    }

    struct sigaction sa = {0};
    sa.sa_handler = handle_sigint;
    sigemptyset(&sa.sa_mask);
    if (sigaction(SIGINT, &sa, NULL) == -1) {
        perror("sigaction");
        return EXIT_FAILURE;
    }

    signal(SIGPIPE, SIG_IGN);

    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == -1) {
        perror("socket");
        return EXIT_FAILURE;
    }

    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons((uint16_t)port);

    if (inet_pton(AF_INET, argv[1], &server_addr.sin_addr) != 1) {
        fprintf(stderr, "Invalid IPv4 address: %s\n", argv[1]);
        close(sock);
        return EXIT_FAILURE;
    }

    if (connect(sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
        perror("connect");
        close(sock);
        return EXIT_FAILURE;
    }

    char buf[MAX_MESSAGE + 256];

    ssize_t n = recv(sock, buf, sizeof(buf) - 1, 0);
    if (n <= 0) {
        fprintf(stderr, "Server closed the connection.\n");
        close(sock);
        return EXIT_FAILURE;
    }
    buf[n] = '\0';
    printf("%s", buf);

    char username[MAX_USERNAME + 1];
    if (!fgets(username, sizeof(username), stdin)) {
        close(sock);
        return EXIT_FAILURE;
    }
    username[strcspn(username, "\r\n")] = '\0';

    char login[MAX_USERNAME + 3];
    snprintf(login, sizeof(login), "%s\n", username);
    if (send_all(sock, login, strlen(login)) < 0) {
        perror("send");
        close(sock);
        return EXIT_FAILURE;
    }

    printf("\nLogin request sent. Welcome, %s!\n", username);
    printf("Type /help for commands.\n\n");

    while (running) {
        fd_set readfds;
        FD_ZERO(&readfds);
        FD_SET(STDIN_FILENO, &readfds);
        FD_SET(sock, &readfds);
        int maxfd = sock;

        int ready = select(maxfd + 1, &readfds, NULL, NULL, NULL);
        if (ready < 0) {
            if (errno == EINTR) continue;
            perror("select");
            break;
        }

        if (FD_ISSET(sock, &readfds)) {
            n = recv(sock, buf, sizeof(buf) - 1, 0);
            if (n <= 0) {
                printf("\n[SERVER] Connection closed.\n");
                break;
            }
            buf[n] = '\0';
            printf("%s", buf);
            fflush(stdout);
        }

        if (FD_ISSET(STDIN_FILENO, &readfds)) {
            if (!fgets(buf, sizeof(buf), stdin)) break;
            if (send_all(sock, buf, strlen(buf)) < 0) {
                perror("send");
                break;
            }
            if (strncmp(buf, "/quit", 5) == 0) break;
        }
    }

    close(sock);
    printf("\nDisconnected.\n");
    return 0;
}
