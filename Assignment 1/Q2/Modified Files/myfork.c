#include <linux/sched/task.h>
#include <linux/kernel.h>

asmlinkage int __x64_sys_myfork(void)
{
    return _do_fork(SIGCHLD, 0,0,NULL,NULL,0);
}
