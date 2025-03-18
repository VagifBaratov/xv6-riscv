#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>

int main(int argc, char *argv[]) {
    int pfd[2];
    pid_t pid;

    if (pipe(pfd) == -1) {
        perror("Error: Failed to create pipe\n");
        exit(1);
    }

    pid = fork();
    if (pid == -1) {
        perror("Error: Failed to fork\n");
        exit(1);
    }

    if (pid == 0) {
        if (close(pfd[1]) < 0) {
            perror( "Error: Failed to close write pipe in child\n");
            exit(1);
        }

        char buffer[128];
        ssize_t bytes_read;
        while ((bytes_read = read(pfd[0], buffer, sizeof(buffer))) > 0) {
            printf("%s", buffer);
        }

        close(pfd[0]); 
        exit(0);
    } else {
        close(pfd[0]);

        for (int i = 1; i < argc; i++) {
            int n = write(pfd[1], argv[i], strlen(argv[i]));
            while (n != 0)
            {
                if (n < 0) {
                    perror("Error: Failed write to pipe\n");
                    exit(1);
                }
                n = write(pfd[1], argv[i] + n, (strlen(argv[i]) - n));
            }
            if (write(pfd[1], "\n", 1) < 0) {
                perror("Error: Failed write to pipe\n");
                exit(1);
            }
        }
        
        if (close(pfd[1]) < 0) {
            perror( "Error: Failed to close write pipe\n");
            exit(1);
        }

        if (wait(0) < 0) {
            perror("Error: Failed to wait\n");
            exit(1);
        }
        exit(0);
    }
}