#include "types.h"
#include "riscv.h"
#include "defs.h"
#include "param.h"
#include "spinlock.h"
#include "proc.h"
#include "fs.h"
#include "sleeplock.h"
#include "file.h"

int
mutexalloc(struct file **f) {
    *f = 0;
    if((*f = filealloc()) == 0)
        goto bad;

    (*f)->mutex = 0;
    if (((*f)->mutex = (struct sleeplock*)kalloc()) == 0)
        goto bad;

    initsleeplock((*f)->mutex, "mutex");
    (*f)->type = FD_MUTEX;
    (*f)->readable = 1;
    (*f)->writable = 0;
    return 0;

bad:
    if((*f)->mutex)
        kfree((char*)(*f)->mutex);
    if(*f)
        fileclose(*f);
    return -1;
}

void
mutexclose(struct file *f) {
    
    if (f->type ==FD_MUTEX){
        kfree((char*)f->mutex);    
    }
    f->type = FD_NONE;
}