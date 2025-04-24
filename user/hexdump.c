#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fcntl.h"

int main(int argc, char *argv[])
{
  if(argc != 3){
    fprintf(2, "Usage: hexdump file count\n");
    exit(1);
  }
  
  int fd = open(argv[1], O_RDONLY);
  if(fd < 0){
    fprintf(2, "hexdump: cannot open %s\n", argv[1]);
    exit(1);
  }
  
  int count = atoi(argv[2]);
  char buf[1];
  
  for(int i = 0; i < count; i++){
    if(read(fd, buf, 1) != 1){
      if(i == 0){
        fprintf(2, "hexdump: read error\n");
        exit(1);
      }
      break;
    }
    int byte = buf[0] & 0xff;
    if (byte <= 0xf) printf("0");
    
    printf("%x ", buf[0] & 0xff);
    if((i+1) % 16 == 0)
      printf("\n");
  }
  close(fd);
  exit(0);
}