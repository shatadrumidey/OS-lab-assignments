#include "types.h"
#include "stat.h"
#include "user.h"

int
main(void)
{
  printf(1, "=== vmprint at startup ===\n");
  vmprint();

  if(sbrk(4 * 4096) == (char*)-1){ printf(1, "sbrk failed\n"); exit(); }
  printf(1, "=== after sbrk(4*4096) ===\n");
  vmprint();

  exit();
}
