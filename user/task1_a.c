#include "user/user.h"

int
main() {

    int pid = fork();
    if (pid < 0) {
        fprintf(2, "Error: Failed to fork\n");
    }
    else if (pid == 0) {
        int n = sleep(15);
        if (n > 0) {
            fprintf(2, "Error: Failed to sleep: process sleped %d\n", 15 - n);
        }
        exit(1);

    } else {
        printf("Parent pid: %d Child pid: %d\n", getpid(), pid);
        int exitCode;

        if (wait(&exitCode) < 0) {
            fprintf(2, "Error: Failed to wait\n");
            exit(1);
        }
        printf("Child %d finished with %d\n", pid, exitCode);
        exit(0);
    }
}