#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fcntl.h"

void test_read_write()
{
    printf("Testing read/write on mutex...\n");
    char buf[1];
    int mx = mutex();
    if(mx < 0) {
        printf("Failed to create mutex\n");
        exit(1);
    }  
     
    if(read(mx, buf, 1) != -1) {
        printf("Error: read from mutex should fail\n");
    } else {
        printf("Read from mutex correctly failed\n");
    }
    
    if(write(mx, "x", 1) != -1) {
        printf("Error: write to mutex should fail\n");
    } else {
        printf("Write to mutex correctly failed\n");
    }
}

void test_close_locked(int is_foreign)
{   
    printf("Testing close of %s mutex...\n", is_foreign ? "foreign locked" : "self locked");
    int mx = mutex();
    if(mx < 0) {
        printf("Failed to create mutex\n");
        exit(1);
    }

    if (!is_foreign) {
        if(mutex_lock(mx) < 0) {
            printf("Failed to lock mutex\n");
            close(mx);
            exit(1);
        }
    }
    
    int pid = fork();
    if(pid < 0) {
        printf("Fork failed\n");
        mutex_unlock(mx);
        close(mx);
        exit(1);
    }
    
    if(pid == 0) {
        if (is_foreign) {
            if(mutex_lock(mx) < 0) {
                printf("Failed to lock mutex\n");
                close(mx);
                exit(1);
            }
            printf("Child locked mutex\n");
            sleep(35);
            printf("Child unlocked mutex\n");
            mutex_unlock(mx);
        }
        exit(0);
    } else {
        sleep(10);
        if(close(mx) < 0) {   
            printf("Error: should be able to close self locked mutex\n");
            exit(0);
        } else {
            if (!is_foreign)
                printf("Successfully closed self locked mutex\n");
            else {
                printf("Successfully can't closed foreign locked mutex before unlock\n");
            }
            return;
        }
    }
}


void test_exit_with_mutex()
{
    printf("Testing process exit with locked mutex...\n");
    int mx = mutex();
    if(mx < 0) {
        printf("Failed to create mutex\n");
        exit(1);
    }
    
    int pid = fork();
    if(pid < 0) {
        printf("Fork failed\n");
        close(mx);
        exit(1);
    }
    
    if(pid == 0) {
        if(mutex_lock(mx) < 0) {
            printf("Failed to lock mutex\n");
            close(mx);
            exit(1);
        }
        exit(0);
    } else {
        wait(0);
        if(mutex_lock(mx) < 0) {
            printf("Error: mutex should be released after process exit\n");
        } else {
            printf("Successfully locked mutex after child exit (mutex was released)\n");
        }
        close(mx);
    }
}

void test_unlock_foreign()
{
    printf("Testing unlocking foreign mutex...\n");
    int mx = mutex();
    if(mx < 0) {
        printf("Failed to create mutex\n");
        return;
    }
    
    int pid = fork();
    if(pid < 0) {
        printf("Fork failed\n");
        close(mx);
        return;
    }
    
    if(pid == 0) {
        if(mutex_lock(mx) < 0) {
            printf("Child failed to lock mutex\n");
            exit(1);
        }
        printf("Child locked mutex, sleeping...\n");
        sleep(35);
        exit(0);
    } else {
        sleep(10);
        
        if(mutex_unlock(mx) == 0) {
            printf("Error: should not be able to unlock foreign mutex\n");
        } else {
            printf("Correctly failed to unlock foreign mutex\n");
        }
        wait(0);
        close(mx);
    }
}

int main(int argc, char *argv[])
{
    test_read_write();
    test_close_locked(0);
    test_close_locked(1);
    test_exit_with_mutex();
    test_unlock_foreign();
    
    printf("All tests completed\n");
    exit(0);
}