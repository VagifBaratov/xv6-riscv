#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

#define PAGE_SIZE 4096
#define ARRAY_SIZE (2 * PAGE_SIZE + 1234) 

int global_var;

void print_pages(char *msg, uint64 buf, int len, int flags) {
  printf("\n%s:\n", msg);
  pagetableinfo(buf, len, flags);
}

int main() {
  print_pages("Initial page table", 0, 0, 0);
  
  print_pages("Before writing to global var", (uint64)&global_var, sizeof(global_var), 0);
  global_var = 1;
  print_pages("After writing to global var", (uint64)&global_var, sizeof(global_var), 0);
 
  int stack_var;
  stack_var = 2;
  print_pages("After writing to stack var", (uint64)&stack_var, sizeof(stack_var), 0);
  
  int stack_array[10];
  stack_array[0] = 3;
  print_pages("After writing to stack array", (uint64)stack_array, sizeof(stack_array), 0);
 
  int *heap_array = malloc(ARRAY_SIZE * sizeof(int));
  heap_array[0] = 4;
  heap_array[PAGE_SIZE/sizeof(int)] = 5;
  heap_array[2*PAGE_SIZE/sizeof(int)] = 6;
  print_pages("After allocating and writing to heap array", (uint64)heap_array, ARRAY_SIZE * sizeof(int), 0);

  pagetableclear(0, 0, 3);
  print_pages("After clearing A and D flags", 0, 0, 0);

  int tmp = global_var + stack_var + stack_array[0] + heap_array[0] + 
            heap_array[PAGE_SIZE/sizeof(int)] + heap_array[2*PAGE_SIZE/sizeof(int)];
  print_pages("After reading data", 0, 0, 2);
  
  heap_array[0] = tmp;
  heap_array[PAGE_SIZE/sizeof(int)] = tmp + 1;
  print_pages("After modifying data", 0, 0, 1);

  free(heap_array);
  print_pages("After freeing heap array", 0, 0, 0);
  return 0;
}