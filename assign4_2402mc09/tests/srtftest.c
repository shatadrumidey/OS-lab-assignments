#include "types.h"
#include "stat.h"
#include "user.h"

static void work(int n){ volatile int i; for(i = 0; i < n; i++); }

int
main(void)
{
  int pidA, pidB, pidC;

  printf(1, "SRTF Test Program Started\n");
  set_burst(getpid(), 80);          // parent stays schedulable during setup

  pidA = fork();
  if(pidA == 0){
    set_burst(getpid(), 400);
    printf(1, "PID %d (burst 400) STARTED at tick %d\n", getpid(), uptime());
    work(400000000);
    printf(1, "PID %d FINISHED at tick %d\n", getpid(), uptime());
    exit();
  }
  set_burst(pidA, 400);
  sleep(15);                        // let the LONG job start first

  pidB = fork();
  if(pidB == 0){
    set_burst(getpid(), 100);
    printf(1, "PID %d (burst 100) STARTED at tick %d\n", getpid(), uptime());
    work(100000000);
    printf(1, "PID %d FINISHED at tick %d\n", getpid(), uptime());
    exit();
  }
  set_burst(pidB, 100);             // short job arrives -> must preempt A
  sleep(5);

  pidC = fork();
  if(pidC == 0){
    set_burst(getpid(), 250);
    printf(1, "PID %d (burst 250) STARTED at tick %d\n", getpid(), uptime());
    work(250000000);
    printf(1, "PID %d FINISHED at tick %d\n", getpid(), uptime());
    exit();
  }
  set_burst(pidC, 250);

  wait(); wait(); wait();
  printf(1, "Expected finish order: %d (100), %d (250), %d (400)\n", pidB, pidC, pidA);
  printf(1, "All processes completed under SRTF scheduler.\n");
  exit();
}
