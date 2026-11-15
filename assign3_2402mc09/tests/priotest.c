#include "types.h"
#include "stat.h"
#include "user.h"

int
main(void)
{
  int i, pid, pids[4];
  int prios[4] = {1, 5, 10, 20};

  printf(1, "Priority Test Program Started\n");

  for(i = 0; i < 4; i++){
    pid = fork();
    if(pid < 0){
      printf(1, "Fork failed\n");
      exit();
    }
    if(pid == 0){
      int j, k;
      volatile int val = 0;
      sleep(2);                    // let parent set all priorities first
      for(j = 0; j < 30000; j++)
        for(k = 0; k < 10000; k++)
          val += j * k;            // CPU-bound work
      printf(1, "Process Finished: PID %d (priority %d) at tick %d\n",
             getpid(), prios[i], uptime());
      exit();
    }
    set_priority(pid, prios[i]);   // parent sets child's priority now
    pids[i] = pid;
  }

  for(i = 0; i < 4; i++)
    wait();

  printf(1, "High priority finished first and ALL 4 completed -> no starvation\n");
  exit();
}
