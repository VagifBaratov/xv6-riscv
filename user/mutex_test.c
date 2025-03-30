#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fcntl.h"

void print_args_unsync(int argc, char *argv[])
{
    for(int i = 1; i < argc; i++) {
        for(char *c = argv[i]; *c != '\0'; c++) {
            char c1[2] = {*c, '\0'};
            printf("%d: arg %d, char %s\n", getpid(), i, c1);
        }
    }
}

void print_args_sync(int argc, char *argv[], int mx)
{
    for(int i = 1; i < argc; i++) {
        for(char *c = argv[i]; *c != '\0'; c++) {
            if(mutex_lock(mx) < 0) {
                printf("Failed to lock mutex\n");
                exit(1);
            }
            char c1[2] = {*c, '\0'};
            printf("%d: arg %d, char %s\n", getpid(), i, c1);
            if(mutex_unlock(mx) < 0) {
                printf("Failed to unlock mutex\n");
                exit(1);
            }
        }
    }
}

int main(int argc, char *argv[])
{
    if(argc < 2) {
        printf("Usage: %s arg1 arg2 ...\n", argv[0]);
        exit(1);
    }

    printf("=== Testing without synchronization ===\n");
    int pid = fork();
    if(pid < 0) {
        printf("Fork failed\n");
        exit(1);
    } else if(pid == 0) {
        print_args_unsync(argc, argv);
        exit(0);
    } else {
        print_args_unsync(argc, argv);
        wait(0);
    }

    printf("\n=== Testing with mutex synchronization ===\n");
    int mx = mutex();
    if(mx < 0) {
        printf("Failed to create mutex\n");
        exit(1);
    }

    pid = fork();
    if(pid < 0) {
        printf("Fork failed\n");
        close(mx);
        exit(1);
    } else if(pid == 0) {
        print_args_sync(argc, argv, mx);
        exit(0);
    } else {
        print_args_sync(argc, argv, mx);
        wait(0);
        close(mx);
    }

    exit(0);
}