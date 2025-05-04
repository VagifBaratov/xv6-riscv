#include "kernel/types.h"
#include "kernel/stat.h"
#include "kernel/fcntl.h"
#include "user/user.h"

int main(int argc, char *argv[]) {
    int fd, r;
    char buf[128];

    fd = open("/dev/null", O_RDWR);
    r = write(fd, "hello", 5);
    
    if (r == 5)
        printf("Success: write to null, returned %d\n", r);
    else
        printf("Failed: write to null, returned %d (expected 5)\n", r);

    r = read(fd, buf, 1);
    if (r == 0)
        printf("Success: read from null, returned %d\n", r);
    else
        printf("Failed: read from null, returned %d (expected 0)\n", r);
    close(fd);

    printf("\n");
    fd = open("/dev/zero", O_RDWR);
    r = read(fd, buf, 3);
    if (r == 3)
        printf("Success: read 3 bytes from zero %d %d %d, returned %d\n", buf[0], buf[1], buf[2], r);
    else
        printf("Failed: read 3 bytes from zero %d %d %d, returned %d (expected 3)\n", buf[0], buf[1], buf[2], r);

    r = write(fd, "smth", 4);
    if (r == -1)
        printf("Success: write to zero, returned %d\n", r);
    else
        printf("Failed: write to zero, returned %d (expected -1)\n", r);
    close(fd);


    printf("\n");
    fd = open("/dev/urandom", O_RDWR);
    r = read(fd, buf, 3);
    if (r == 3)
        printf("Success: read 3 bytes from urandom %d %d %d, returned %d\n", buf[0], buf[1], buf[2], r);
    else    
        printf("Failed: read 3 bytes from urandom %d %d %d, returned %d (expected 3)\n", buf[0], buf[1], buf[2], r);

    uint64 seed = 12345678;
    r = write(fd, &seed, sizeof(seed));
    if (r == 8)
        printf("Success: write to urandom, returned %d\n", r);
    else
        printf("Failed: write to urandom, returned %d (expected 8)\n", r);

    r = read(fd, buf, 3);
    if (r == 3)
        printf("Success: read 3 bytes from urandom %d %d %d after setting new seed, returned %d\n", buf[0], buf[1], buf[2], r);
    else    
        printf("Failed: read 3 bytes from urandom %d %d %d after setting new seed, returned %d (expected 3)\n", buf[0], buf[1], buf[2], r);
    close(fd);


    printf("\n");
    fd = open("/dev/nullstat", O_RDWR);
    r = write(fd, "abc", 3);

    if (r == 3)
        printf("Success: write to nullstat, returned %d\n", r);
    else
        printf("Failed: write to nullstat, returned %d (expected 3)\n", r);
    
    r = write(fd, "defgh", 5);
    if (r == 5)
        printf("Success: write to nullstat, returned %d\n", r);
    else
        printf("Failed: write to nullstat, returned %d (expected 5)\n", r);

    uint64 count;
    r = read(fd, &count, sizeof(count));
    if (r == sizeof(count))
        if (r == 8)
            printf("Success: read from nullstat, returned %d\n", (int)count);
        else
            printf("Failed: read from nullstat, returned %d (expected 8)\n", (int)count);
    else
        printf("error reading from nullstat\n");
    
    r = read(fd, &count, 4);
    if (r == -1)
        printf("Success: read with wrong size from nullstat, returned %d\n", r);
    else
        printf("Failed: read with wrong size from nullstat, returned %d (expected -1)\n", r);
    close(fd);

    exit(0);
}