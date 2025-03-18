#include "user/user.h"
#include "kernel/procinfo.h"

const char* state_to_str(int state) {
  switch (state) {
    case 0: return "UNUSED ";
    case 1: return "EMBRYO ";
    case 2: return "SLEEP  ";
    case 3: return "RUNNABL";
    case 4: return "RUNNING";
    case 5: return "ZOMBIE ";
    default: return "?????? ";
  }
}

int main() {
  int count = ps_listinfo(0, 0);
  if (count < 0) {
    printf("ps: ошибка %d\n", count);
    exit(1);
  }

  struct procinfo *procs = malloc(count * sizeof(struct procinfo));
  if (!procs) {
    printf("ps: ошибка выделения памяти\n");
    exit(1);
  }

  int ret = ps_listinfo(procs, count);
  if (ret != count) {
    printf("ps: ошибка %d\n", ret);
    free(procs);
    exit(1);
  }

  printf("   ID | STATE    | NAME     | PPID | PARENT\n");
  printf("------+----------+----------+------+-------\n");

  for (int i = 0; i < count; i++) {
    struct procinfo *p = &procs[i];
    char pname[16] = "none";

    if (p->ppid != -1) {
      struct procinfo parent;
      if (getprocinfo(p->ppid, &parent) == 0) {
        strcpy(pname, parent.name);
      } else {
        strcpy(pname, "zombie?");
      }
    }

    printf("%d | %s | %s | %d | %s\n",
           p->pid, state_to_str(p->state), p->name, p->ppid, pname);
  }

  free(procs);
  exit(0);
}