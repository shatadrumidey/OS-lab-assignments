#include "types.h"
#include "stat.h"
#include "user.h"

static void work(int n){ volatile int i; for(i = 0; i < n; i++); }
static void delay(int n){ volatile int i; for(i = 0; i < n; i++); }

int
main(void)
{
  int pidL, pidM, pidS;

  printf(1, "HRRN Test Program Started\n");

  pidL = fork();
  if(pidL == 0){
    work(300000000);                       // very long
    printf(1, "PID %d (burst 1500, LONG) FINISHED at tick %d\n", getpid(), uptime());
    exit();
  }
  set_burst(pidL, 1500);
  delay(2000000);

  pidM = fork();
  if(pidM == 0){
    work(80000000);
    printf(1, "PID %d (burst 300, medium) FINISHED at tick %d\n", getpid(), uptime());
    exit();
  }
  set_burst(pidM, 300);
  delay(2000000);

  pidS = fork();
  if(pidS == 0){
    work(30000000);
    printf(1, "PID %d (burst 100, SHORT) FINISHED at tick %d\n", getpid(), uptime());
    exit();
  }
  set_burst(pidS, 100);

  wait(); wait(); wait();
  printf(1, "Expected finish order: %d (short), %d (medium), %d (long)\n", pidS, pidM, pidL);
  printf(1, "All processes completed under HRRN scheduler (no starvation).\n");
  exit();
}
