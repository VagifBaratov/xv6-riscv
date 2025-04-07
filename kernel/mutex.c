#include "types.h"
#include "riscv.h"
#include "defs.h"
#include "param.h"
#include "spinlock.h"
#include "proc.h"
#include "fs.h"
#include "sleeplock.h"
#include "file.h"

#define mutex_debug(fmt, ...) \
    printf("mutex [pid %d]: %s: " fmt, myproc()->pid, __func__, ##__VA_ARGS__)

int
mutexalloc(struct file **f) {
    *f = 0;
    mutex_debug("allocating new mutex\n");
    
    if((*f = filealloc()) == 0) {
        mutex_debug("filealloc failed\n");
        goto bad;
    }

    (*f)->mutex = 0;
    mutex_debug("allocating sleeplock memory\n");
    if (((*f)->mutex = (struct sleeplock*)kalloc()) == 0) {
        mutex_debug("kalloc failed for sleeplock\n");
        goto bad;
    }
    mutex_debug("allocated sleeplock at %p\n", (*f)->mutex);

    initsleeplock((*f)->mutex, "mutex");
    (*f)->type = FD_MUTEX;
    (*f)->readable = 0;
    (*f)->writable = 0;

    mutex_debug("successfully allocated mutex at %p\n", *f);
    return 0;

bad:
    mutex_debug("cleaning up after failure\n");
    if((*f)->mutex) {
        mutex_debug("freeing sleeplock at %p\n", (*f)->mutex);
        kfree((char*)(*f)->mutex);
    }
    if(*f){
        mutex_debug("closing file at %p\n", *f);
        fileclose(*f);
    }
    return -1;
}

void
mutexclose(struct file *f) {
    
    mutex_debug("closing mutex at %p\n", f);

    if (f->type ==FD_MUTEX){
        mutex_debug("freeing sleeplock at %p\n", f->mutex);
        kfree((char*)f->mutex);    
    }
    f->type = FD_NONE;
}