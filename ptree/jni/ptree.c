// this is the source file to test the syscall ptree we implemented in problem1
// Author:520021911141 WeimingZhang

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define __NR_ptreecall 356
#define BUFFER_MAX 1024

struct prinfo
{
  pid_t parent_pid;       /* process id of parent */
  pid_t pid;              /* process id */
  pid_t first_child_pid;  /* pidof youngest child */
  pid_t next_sibling_pid; /* pidof older sibling */
  long state;             /* current state of process */
  long uid;               /* user id of process owner */
  char comm[64];          /* name of program executed */
  // add a new variable to record the depth of the process in the ptree
  int depth; /* number of tabs to printf/printk; can be used for ptree program*/
};

int main()
{
  int *nr = malloc(sizeof(int));
  // struct prinfo *buf = malloc(BUFFER_MAX * sizeof(struct prinfo));
  struct prinfo buf[BUFFER_MAX];
  struct prinfo *bp = buf;
  (*nr) = BUFFER_MAX;
  // printf("*nr=:%d\n", *nr);
  if (bp == NULL || nr == NULL)
  {
    printf("Memory Allocation Error!");
    return -1;
  }
  // the syscall returns the number of processes
  int pc = syscall(__NR_ptreecall, bp, nr);
  printf("Number of process: %d\n", pc);
  printf("Name\tPID\tState\tPPID\tFCPID\tNSPID\tUID\n");
  int i, j;
  for (i = 0; i < pc; ++i)
  {
    // print '\t' to show the depth of the process
    for (j = 0; j < buf[i].depth; ++j)
      printf("\t");
    printf("%s\t%d\t%ld\t%d\t%d\t%d\t%ld\n", buf[i].comm, buf[i].pid,
           buf[i].state, buf[i].parent_pid, buf[i].first_child_pid,
           buf[i].next_sibling_pid, buf[i].uid);
  }

  return 0;
}