#include "user/user.h"

int
main(int argc, char **argv) {
    int pfd[2];

    if (pipe(pfd) < 0) {
        fprintf(2, "Error: Failed to create pipe\n");
        exit(1);
    }

    int pid = fork();
    if (pid < 0) {
        fprintf(2, "Error: Failed to fork\n");
        exit(1);
    }
    else if (pid == 0) {
        
        if (close(pfd[1]) < 0) {
            fprintf(2, "Error: Failed to close write pipe in child\n");
            exit(1);
        }
        if (close(0) < 0) {
            fprintf(2, "Error: Failed to close cin\n");
            exit(1);
        }
        if(dup(pfd[0]) < 0) {
            fprintf(2, "Error: Failed to dup pipe[0]\n");
            exit(1);
        }
        
        close(pfd[0]);

        char *argv[] = {"/wc", 0};
        exec("/wc", argv);

        fprintf(2, "Error: Failed to exec wc\n");
        exit(1);
    }
    else {
        close(pfd[0]);
        
    for (int i = 1; i < argc; i++) {
            int n = write(pfd[1], argv[i], strlen(argv[i]));
            while (n != 0) {
                if (n < 0)
                {
                    fprintf(2, "Error: Failed write to pipe\n");
                    exit(1);
                }
                n = write(pfd[1], argv[i] + n, (strlen(argv[i]) - n));
            }
            
            if (write(pfd[1], "\n", 1) < 0)
            {
                fprintf(2, "Error: Failed write to pipe\n");
                exit(1);
            }
    }
    if (close(pfd[1]) < 0) {
        fprintf(2, "Error: Failed to close write pipe\n");
        exit(1);
    }
        
    if (wait(0) < 0) {
        fprintf(2, "Error: Failed to wait\n");
        exit(1);
    }
        exit(0);
    }
}