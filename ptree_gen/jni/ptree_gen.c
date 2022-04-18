// this is the source file of generating a child process
// the process forks a child process to call ptree syscall
// Author:520021911141 WeimingZhang
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
int main() {
  // pid_t pid = getpid();
  pid_t pid;
  // fork a child process
  pid = fork();
  if (pid < 0) {
    printf("Create child process failed\n");
    return -1;
  }
  // pid==0, that is the child process
  // print the information and
  else if (pid == 0) {
    printf("StudentID:520021911141\tChild PID is: %d\n", getpid());
    // execute the ptree program and test syscall ptree
    execl("/data/misc/pstree/ptree", "ptree", NULL);

  } else {
    printf("StudentID:520021911141\tParent PID is: %d\n", getpid());
    wait(NULL);
  }
}
