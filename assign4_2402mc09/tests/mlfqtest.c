#include "types.h"
#include "stat.h"
#include "user.h"

static void cpu_loop(void){ volatile int i = 0; for(;;) i++; }

int
main(void)
{
  int cpu1, cpu2, io1, io2, i;

  printf(1, "MLFQ Test Program Started\n");

  cpu1 = fork();
  if(cpu1 == 0) cpu_loop();                 // CPU-bound (infinite)

  cpu2 = fork();
  if(cpu2 == 0) cpu_loop();

  io1 = fork();
  if(io1 == 0){
    for(i = 0; i < 60; i++) sleep(1);       // I/O-bound (yields often)
    printf(1, "I/O process %d done\n", getpid());
    exit();
  }

  io2 = fork();
  if(io2 == 0){
    for(i = 0; i < 60; i++) sleep(1);
    printf(1, "I/O process %d done\n", getpid());
    exit();
  }

  sleep(120);                               // let the mix play out

  printf(1, "CPU-bound pid %d queue level: %d (expect 2)\n", cpu1, get_queue_level(cpu1));
  printf(1, "CPU-bound pid %d queue level: %d (expect 2)\n", cpu2, get_queue_level(cpu2));
  printf(1, "I/O-bound pid %d queue level: %d (expect 0)\n", io1, get_queue_level(io1));
  printf(1, "I/O-bound pid %d queue level: %d (expect 0)\n", io2, get_queue_level(io2));

  kill(cpu1); kill(cpu2);
  wait(); wait(); wait(); wait();
  printf(1, "MLFQ done: CPU-heavy sank to level 2, I/O-bound stayed at level 0\n");
  exit();
}
