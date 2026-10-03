#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

int main() {
    char input[100];

    printf("====================================\n");
    printf("       MY CUSTOM LINUX SHELL        \n");
    printf("====================================\n");

    printf("Parent Process ID: %d\n", getpid());
    printf("Parent's Parent Process ID: %d\n", getppid());

    while (1) {

        // Display custom prompt
        printf("bharath-shell> ");
        fflush(stdout);

        // Read user input
        if (fgets(input, sizeof(input), stdin) == NULL) {
            printf("\nExiting...\n");
            break;
        }

        // Remove newline
        input[strcspn(input, "\n")] = '\0';

        // Exit condition
        if (strcmp(input, "exit") == 0) {
            printf("Exiting shell...\n");
            break;
        }

        // Ignore empty input
        if (strlen(input) == 0) {
            continue;
        }

        // Create child process
        pid_t pid = fork();

        if (pid < 0) {
            perror("fork failed");
            continue;
        }

        // Child process
        if (pid == 0) {

            printf("[Child Process] PID = %d\n", getpid());
            printf("[Child Process] Parent PID = %d\n", getppid());

            // Execute Linux command
            execlp("sh", "sh", "-c", input, (char *)NULL);

            // Runs only if command execution fails
            perror("Command failed");
            exit(1);
        }

        // Parent process
        else {
            printf("[Parent Process] PID = %d\n", getpid());
            printf("[Parent Process] Child PID = %d\n", pid);

            wait(NULL);

            printf("[Parent] Child process completed.\n");
        }
    }

    return 0;
}
