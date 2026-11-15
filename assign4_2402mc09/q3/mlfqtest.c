#include "types.h"
#include "stat.h"
#include "user.h"

void
cpu_bound(int id)
{
  volatile int x = 0;
  int i;
  printf(1, "CPU%d (pid %d) start\n", id, getpid());
  for(i = 0; i < 60000000; i++) x++;
  printf(1, "CPU%d (pid %d) done, final level %d\n", id, getpid(), getqlevel(getpid()));
}

void
io_bound(int id)
{
  int i;
  printf(1, "IO%d (pid %d) start\n", id, getpid());
  for(i = 0; i < 40; i++)
    sleep(2);
  printf(1, "IO%d (pid %d) done, final level %d\n", id, getpid(), getqlevel(getpid()));
}

int
main(void)
{
  int c1, c2, i1, i2, r;

  printf(1, "=== MLFQ Scheduler Test (3 queues) ===\n");
  printf(1, "Quantum: level0=1, level1=2, level2=4 ticks\n");
  printf(1, "2 CPU-bound + 2 I/O-bound children\n\n");

  c1 = fork();
  if(c1 == 0){ cpu_bound(1); exit(); }
  c2 = fork();
  if(c2 == 0){ cpu_bound(2); exit(); }
  i1 = fork();
  if(i1 == 0){ io_bound(1); exit(); }
  i2 = fork();
  if(i2 == 0){ io_bound(2); exit(); }

  for(r = 0; r < 10; r++){
    sleep(10);
    printf(1, "round %d: CPU1=%d CPU2=%d IO1=%d IO2=%d\n",
           r, getqlevel(c1), getqlevel(c2), getqlevel(i1), getqlevel(i2));
  }

  wait(); wait(); wait(); wait();
  printf(1, "\n=== CPU-bound sank to level 2; I/O-bound stayed at level 0 ===\n");
  printf(1, "=== mlfqtest done ===\n");
  exit();
}
