#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main() {
    pid_t pid;

    pid = fork();

    if (pid < 0) {
        printf("Fork Failed!\n");
        return 1;
    }
    else if (pid == 0) {
        printf("This is Child Process\n");
        printf("Child PID = %d\n", getpid());

        char *args[] = {"ls", "-l", NULL};
        execvp("ls", args);

        printf("Exec failed!\n");
    }
    else {
        wait(NULL);
        printf("\nThis is Parent Process\n");
        printf("Parent PID = %d\n", getpid());
        printf("Child PID = %d\n", pid);
    }

    return 0;
}
