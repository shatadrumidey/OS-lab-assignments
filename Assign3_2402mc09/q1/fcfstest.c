#include "types.h"
#include "stat.h"
#include "user.h"

void
cpu_work(int id)
{
  volatile int x = 0;
  int i;
  printf(1, "Process %d (pid %d) starting CPU work\n", id, getpid());
  for(i = 0; i < 200000000; i++) x++;
  printf(1, "Process %d (pid %d) finished\n", id, getpid());
}

int
main(void)
{
  int pid1, pid2, pid3;
  int i;

  printf(1, "=== FCFS Scheduler Test ===\n\n");

  pid1 = fork();
  if(pid1 == 0){ cpu_work(1); exit(); }

  for(i = 0; i < 50000000; i++);

  pid2 = fork();
  if(pid2 == 0){ cpu_work(2); exit(); }

  for(i = 0; i < 50000000; i++);

  pid3 = fork();
  if(pid3 == 0){ cpu_work(3); exit(); }

  wait();
  wait();
  wait();

  printf(1, "\n=== Completion order should be: 1, 2, 3 ===\n");
  printf(1, "=== fcfstest done ===\n");
  exit();
}
