#include "types.h"
#include "riscv.h"
#include "defs.h"
#include "param.h"
#include "memlayout.h"
#include "spinlock.h"
#include "proc.h"
#include "procinfo.h"
uint64
sys_exit(void)
{
  int n;
  argint(0, &n);
  exit(n);
  return 0;  // not reached
}

uint64
sys_getpid(void)
{
  return myproc()->pid;
}

uint64
sys_fork(void)
{
  return fork();
}

uint64
sys_wait(void)
{
  uint64 p;
  argaddr(0, &p);
  return wait(p);
}

uint64
sys_sbrk(void)
{
  uint64 addr;
  int n;

  argint(0, &n);
  addr = myproc()->sz;
  if(growproc(n) < 0)
    return -1;
  return addr;
}

uint64
sys_sleep(void)
{
  int n;
  uint ticks0;

  argint(0, &n);
  if(n < 0)
    n = 0;
  acquire(&tickslock);
  ticks0 = ticks;
  while(ticks - ticks0 < n){
    if(killed(myproc())){
      release(&tickslock);
      return -1;
    }
    sleep(&ticks, &tickslock);
  }
  release(&tickslock);
  return 0;
}

uint64
sys_kill(void)
{
  int pid;

  argint(0, &pid);
  return kill(pid);
}

// return how many clock tick interrupts have occurred
// since start.
uint64
sys_uptime(void)
{
  uint xticks;

  acquire(&tickslock);
  xticks = ticks;
  release(&tickslock);
  return xticks;
}



int 
sys_getprocinfo(void) {
    int pid;
    uint64 user_info_addr;
    struct procinfo kernel_info;
    struct proc *p;
    struct proc *current_proc = myproc();

    argint(0, &pid);        
    argaddr(1, &user_info_addr);

    if (user_info_addr != 0 && 
       (user_info_addr >= current_proc->sz || 
        user_info_addr + sizeof(struct procinfo) > current_proc->sz)) {
        return -1; 
    }

    for (p = proc; p < &proc[NPROC]; p++) {
        acquire(&p->lock);
        if (p->pid == pid && p->state != UNUSED) {
            kernel_info.pid = p->pid;
            kernel_info.state = p->state;
            strncpy(kernel_info.name, p->name, 16);

            acquire(&wait_lock);
            kernel_info.ppid = p->parent ? p->parent->pid : -1;
            release(&wait_lock);

            int error = 0;
            if (user_info_addr != 0) {
                if (copyout(current_proc->pagetable, user_info_addr, 
                           (char*)&kernel_info, sizeof(kernel_info)) < 0) {
                    error = -1; 
                }
            }
            release(&p->lock);
            return error;
        }
        release(&p->lock);
    }

    return -1; 
}
int 
sys_ps_listinfo(void) {
    uint64 user_plist_addr;
    int lim;
    struct proc *p;
    int total = 0;
    int index = 0;
    int error = 0;

    argaddr(0, &user_plist_addr);
    argint(1, &lim);

    struct procinfo *user_plist = (struct procinfo *)user_plist_addr;
    struct proc *current_proc = myproc();

    if (user_plist == 0) {
        for (p = proc; p < &proc[NPROC]; p++) {
            acquire(&p->lock);
            if (p->state != UNUSED) total++;
            release(&p->lock);
        }
        return total;
    }

    if (lim < 0) return -2;
    if (user_plist_addr >= current_proc->sz || 
        user_plist_addr + lim * sizeof(struct procinfo) > current_proc->sz) {
        return -3;
    }

    for (p = proc; p < &proc[NPROC] && !error; p++) {
        acquire(&p->lock);
        if (p->state == UNUSED) {
            release(&p->lock);
            continue;
        }

        total++; 

        if (index < lim) {
            struct procinfo info;
            struct proc *parent;

            info.pid = p->pid;
            info.state = p->state;
            strncpy(info.name, p->name, 16);

            acquire(&wait_lock);
            parent = p->parent;
            info.ppid = parent ? parent->pid : -1;
            release(&wait_lock);

            uint64 elem_addr = user_plist_addr + index * sizeof(struct procinfo);
            if (copyout(current_proc->pagetable, elem_addr, (char*)&info, sizeof(info)) < 0) {
                error = -5;
            } else {
                index++;
            }
        }
        release(&p->lock);
    }

    if (!error && total > lim) {
        error = -4;
    }

    return error ? error : (total > lim ? -4 : index);
}