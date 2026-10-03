#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>

int main() {
    pid_t pid;

    pid = fork();

    if (pid < 0) {
        printf("Fork Failed!\n");
    }
    else if (pid == 0) {
        printf("This is the Child Process\n");
        printf("Child PID = %d\n", getpid());

        execl("/bin/ls", "ls", "-l", NULL);

        printf("Exec Failed!\n");
    }
    else {
        printf("This is the Parent Process\n");
        printf("Parent PID = %d\n", getpid());
        printf("Child PID = %d\n", pid);
    }

    return 0;
}
