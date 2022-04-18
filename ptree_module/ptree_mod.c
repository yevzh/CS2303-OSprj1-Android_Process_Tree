// This is the source file of ptree module
// the implementation of ptree syscall
// the main task is to do a deep-first-search in the task_struct structure
// recording the current processes
// Author:520021911141 WeimingZhang
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/sched.h>
#include <linux/unistd.h>
//#include <stdlib.h>
MODULE_LICENSE("Dual BSD/GPL");

#define __NR_ptreecall 356
#define BUFFER_MAX 1024
// prinfo is to store the information in the task_struct
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

static int (*oldcall)(void);

// the function to get information of all processes and store them in buf
//*pc is to count the current number of process we've visited
// the dfs starts from &init_task
static void ptree_dfs(struct prinfo *buf, int *nr, struct task_struct *tsk,
                      int *pd, int *pc)
{
  // exceed the maximum
  if (*pc >= *nr)
    return;
  // if no parent set it to 0
  buf[*pc].parent_pid = (tsk->parent) ? tsk->real_parent->pid : 0;
  buf[*pc].pid = tsk->pid;
  // list_empty to check whether the linked list of child processes is empty
  // list_entry to get the first child's pid
  if (!list_empty(&(tsk->children)))
    buf[*pc].first_child_pid =
        list_entry(tsk->children.next, struct task_struct, sibling)->pid;
  // if no child, set it to 0
  else
    buf[*pc].first_child_pid = 0;
  // it should have a parent, if not, we set it to 0
  // if the next sibling is its parent's last child, then set it to 0
  if (tsk->parent && tsk->sibling.next != &(tsk->parent->children))
    buf[*pc].next_sibling_pid =
        list_entry(tsk->sibling.next, struct task_struct, sibling)->pid;
  else
    buf[*pc].next_sibling_pid = 0;

  buf[*pc].state = tsk->state;
  buf[*pc].uid = tsk->real_cred->uid;
  strcpy(buf[*pc].comm, tsk->comm);
  buf[*pc].depth = *pd;
  // count the process
  ++(*pc);
  if (!list_empty(&(tsk->children)))
  {
    struct task_struct *pt =
        list_entry(tsk->children.next, struct task_struct, sibling);
    // begin dfs
    while (true)
    {
      ++(*pd);
      ptree_dfs(buf, nr, pt, pd, pc);
      --(*pd);
      // if no sibling, the same method as above
      if (pt->sibling.next == &(pt->parent->children))
        break;
      // pt points to sibling and continue dfs
      pt = list_entry(pt->sibling.next, struct task_struct, sibling);
    }
  }
}
// buf is the struct array to store process information
//*nr is the maximum number
static int ptree(struct prinfo *buf, int *nr)
{
  if (buf == NULL || nr == NULL)
  {
    printk("INVALID BUFFER!\n");
    return 0;
  }
  // to count the total processes
  int count = 0;
  // use a point to share count in other functions
  int *p_count = &count;
  // to count the depth of the process in the process tree
  int depth = 0;
  // a point to share it
  int *p_dep = &depth;
  read_lock(&tasklist_lock);
  // dfs to store the information in the buf
  // we start at &init_task
  ptree_dfs(buf, nr, &init_task, p_dep, p_count);
  read_unlock(&tasklist_lock);
  printk("system call successfully end!\n");
  return count;
}
static int addsyscall_init(void)
{
  long *syscall = (long *)0xc000d8c4;
  oldcall = (int (*)(void))(syscall[__NR_ptreecall]);
  syscall[__NR_ptreecall] = (unsigned long)ptree;
  printk(KERN_INFO "module load!\n");
  return 0;
}
static void addsyscall_exit(void)
{
  long *syscall = (long *)0xc000d8c4;
  syscall[__NR_ptreecall] = (unsigned long)oldcall;
  printk(KERN_INFO "module exit!\n");
}

module_init(addsyscall_init);
module_exit(addsyscall_exit);
