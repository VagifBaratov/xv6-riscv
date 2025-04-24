#include "types.h"
#include "param.h"
#include "riscv.h"
#include "spinlock.h"
#include "sleeplock.h"
#include "fs.h"
#include "file.h"
#include "defs.h"

#define NULL_DEV    0
#define ZERO_DEV    1
#define URANDOM_DEV 2
#define NULLSTAT_DEV 3

struct {
  struct spinlock lock;
  uint64 seed;
  uint64 bytes_written;
} pseudo_dev;

int pseudodev_read(int user_dst, uint64 dst, int n, short minor)
{
  acquire(&pseudo_dev.lock);

  switch(minor) {
    case NULL_DEV:
      release(&pseudo_dev.lock);
      return 0;
    
    case ZERO_DEV:
      if(dst == 0 || n <= 0) {
        release(&pseudo_dev.lock);
        return 0;
      }
      
      for(int i = 0; i < n; i++) {
        int nll = 0;
        if (either_copyout(user_dst, dst + i, &nll, 1) < 0) {
          release(&pseudo_dev.lock);
          return -1;
        }
      }
      release(&pseudo_dev.lock);
      return n;
      
    case URANDOM_DEV: {
      if(dst == 0 || n <= 0) {
        release(&pseudo_dev.lock);
        return 0;
      }
      
      for(int i = 0; i < n; i++) {
        pseudo_dev.seed = pseudo_dev.seed * 1664525 + 1013904223;
        uint8 rand = (uint8)(pseudo_dev.seed >> 16);
        if (either_copyout(user_dst, dst + i, &rand, 1) < 0) {
          release(&pseudo_dev.lock);
          return -1;
        }
      }
      
      release(&pseudo_dev.lock);
      return n;
    }
    
    case NULLSTAT_DEV:
      if(n != sizeof(uint64)) {
        release(&pseudo_dev.lock);
        return -1;
      }

      if (either_copyout(user_dst, dst, &pseudo_dev.bytes_written, n) < 0) {
        release(&pseudo_dev.lock);
        return -1;
      }
      
      release(&pseudo_dev.lock);
      return n;
    
    default:
      release(&pseudo_dev.lock);
      return -1;
  }
}

int pseudodev_write(int user_src, uint64 src, int n, short minor)
{
  acquire(&pseudo_dev.lock);
  switch(minor) {
    case NULL_DEV:
      release(&pseudo_dev.lock);
      return n; 
    
    case ZERO_DEV:
      release(&pseudo_dev.lock);
      return -1; 
    
    case URANDOM_DEV:
      if(n != sizeof(uint64)) {
        release(&pseudo_dev.lock);
        return -1;
      }
      
      uint64 new_seed;
      if (either_copyin(&new_seed, user_src, src, n) < 0) {
        release(&pseudo_dev.lock);
        return -1;
      }

      pseudo_dev.seed = new_seed;
      release(&pseudo_dev.lock);
      return n;
    
    case NULLSTAT_DEV:
      pseudo_dev.bytes_written += n;
      release(&pseudo_dev.lock);
      return n;
    
    default:
      release(&pseudo_dev.lock);
      return -1;
  }
}

void pseudodev_init()
{
  initlock(&pseudo_dev.lock, "pseudo_dev");
  pseudo_dev.seed = 123456789;
  pseudo_dev.bytes_written = 0;
  devsw[PSEUDODEV].read = pseudodev_read;
  devsw[PSEUDODEV].write = pseudodev_write;
}