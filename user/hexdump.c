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
 
  static char buf[512]; 
  int bytes_read;
  int bytes_printed = 0;

  while(bytes_printed < count){
    int chunk = (count - bytes_printed > sizeof(buf)) ? sizeof(buf) : (count - bytes_printed);
    bytes_read = read(fd, &buf, chunk);
    if(bytes_read < 0){
      fprintf(2, "hexdump: read error\n");
      exit(1);
    }
    if(bytes_read == 0){
      break;
    }

    for(int i = 0; i < bytes_read && bytes_printed < count; i++){
      if(buf[i] <= 0xf) printf("0");
      printf("%x ", buf[i]);
      bytes_printed++;
      
      if(bytes_printed % 16 == 0)
        printf("\n");
    }
  }

  if(bytes_printed % 16 != 0)
    printf("\n");

  close(fd);
  exit(0);
}