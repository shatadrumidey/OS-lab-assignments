#include "types.h"
#include "stat.h"
#include "user.h"

void
work(int id, int bt)
{
  volatile int x = 0;
  int i;
  set_burst(getpid(), bt);
  printf(1, "P%d (pid %d, burst %d) start\n", id, getpid(), bt);
  for(i = 0; i < 100000000; i++) x++;
  printf(1, "P%d (pid %d, burst %d) done\n", id, getpid(), bt);
}

int
main(void)
{
  int i;
  printf(1, "=== SRTF Scheduler Test ===\n");
  printf(1, "Forking long job (100), then short jobs (10, 5)\n\n");

  if(fork() == 0){ work(1, 100); exit(); }
  for(i=0;i<50000000;i++);
  if(fork() == 0){ work(2, 10); exit(); }
  for(i=0;i<50000000;i++);
  if(fork() == 0){ work(3, 5); exit(); }

  wait(); wait(); wait();
  printf(1, "\n=== Expected: Short jobs preempt long job ===\n");
  printf(1, "=== srfttest done ===\n");
  exit();
}
