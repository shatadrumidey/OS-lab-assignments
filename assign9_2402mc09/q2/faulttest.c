#include "types.h"
#include "stat.h"
#include "user.h"

int
main(void)
{
  char *big;
  int pid;

  big = sbrk(32 * 4096);
  if(big == (char*)-1){ printf(1, "sbrk failed\n"); exit(); }
  printf(1, "sbrk(32 pages) succeeded, faults so far: %d\n", getfaults());

  big[0] = 'A';
  big[4096] = 'B';
  big[8192] = 'C';
  big[12288] = 'D';
  big[16384] = 'E';
  printf(1, "touched 5 pages, faults now: %d (expect 5)\n", getfaults());
  printf(1, "read back: %c %c %c %c %c\n",
         big[0], big[4096], big[8192], big[12288], big[16384]);

  pid = fork();
  if(pid == 0){
    printf(1, "child touching va == sz (invalid)...\n");
    big[32 * 4096] = 'X';
    printf(1, "SHOULD NEVER PRINT\n");
    exit();
  }
  wait();
  printf(1, "parent alive after child killed for bad access\n");
  printf(1, "faulttest done\n");
  exit();
}
