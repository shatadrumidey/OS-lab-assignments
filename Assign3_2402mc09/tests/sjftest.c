#include "types.h"
#include "stat.h"
#include "user.h"

int
main(void)
{
  int i, pid, pids[4];
  int bursts[4] = {400, 100, 300, 200};   // created in this order on purpose

  printf(1, "SJF Test Program Started\n");

  for(i = 0; i < 4; i++){
    pid = fork();
    if(pid < 0){
      printf(1, "Fork failed\n");
      exit();
    }
    if(pid == 0){
      int j, k;
      volatile int val = 0;
      for(j = 0; j < 30000; j++)
        for(k = 0; k < 10000; k++)
          val += j * k;                    // CPU-bound work
      printf(1, "Process Finished: PID %d\n", getpid());
      exit();
    }
    set_burst(pid, bursts[i]);             // parent declares child's burst NOW
    pids[i] = pid;
  }

  for(i = 0; i < 4; i++)
    wait();

  printf(1, "Expected finish order: PID %d, %d, %d, %d\n",
         pids[1], pids[3], pids[2], pids[0]);
  printf(1, "All processes completed under SJF scheduler.\n");
  exit();
}
