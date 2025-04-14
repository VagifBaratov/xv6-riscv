#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

#define PAGE_SIZE 4096
#define ARRAY_SIZE (2 * PAGE_SIZE + 1234) 
#define A 1
#define D 2

void perror(const char *str) {
  write(2, str, strlen(str));
  exit(1);
}

void print_pages(char *msg, uint64 buf, int len, int flags) {
  printf("\n%s:\n", msg);
  pagetableinfo(buf, len, flags);
}

int main() {
  print_pages("Initial page table", 0, 0, 0);
  
  char var = 33;
  char stack_array[ARRAY_SIZE ];
  stack_array[0] = 'a';

  print_pages("After writing to stack var", (uint64)&var, 1, D);
  print_pages("After writing to stack array", (uint64)stack_array, sizeof(stack_array), A);
  
  char *arr = malloc(ARRAY_SIZE);
  if ((uint64)arr == -1) {
    perror("Failed allocate memory");
  }

  print_pages("After allocating heap array", (uint64)arr, ARRAY_SIZE, A | D);

  pagetableclear((uint64)arr, ARRAY_SIZE, A | D);
  print_pages("After clearing A and D flags", (uint64)arr, ARRAY_SIZE, A | D);

  char val = arr[0];
  (void)val;

  print_pages("After reading data", (uint64)stack_array, ARRAY_SIZE, A | D);

  arr[0] = 'B';
  arr[2000] = 'C';
  
  print_pages("After modifying data", (uint64)arr, ARRAY_SIZE, A | D);

  free(arr);
  print_pages("After freeing heap array", (uint64)arr, ARRAY_SIZE, A | D);
  
  print_pages("Pages with A", 0, 0, A);

  print_pages("Pages with D", 0, 0, D);

  print_pages("Pages with A|D", 0, 0, A | D);
  return 0;
}