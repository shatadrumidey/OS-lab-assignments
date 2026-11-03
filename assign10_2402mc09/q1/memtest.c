#include "types.h"
#include "stat.h"
#include "user.h"

int
main(void)
{
  char *p;

  printf(1, "=== Initial state ===\n");
  memstats();

  printf(1, "\nAllocating 8 pages via sbrk...\n");
  p = sbrk(8 * 4096);
  if(p == (char*)-1){ printf(1, "sbrk failed\n"); exit(); }
  printf(1, "=== After sbrk(+8 pages) ===\n");
  memstats();

  printf(1, "\nReleasing 8 pages via sbrk(-8*4096)...\n");
  if(sbrk(-8 * 4096) == (char*)-1){ printf(1, "sbrk shrink failed\n"); exit(); }
  printf(1, "=== After sbrk(-8 pages) ===\n");
  memstats();

  printf(1, "\nAllocating 16 pages via sbrk...\n");
  p = sbrk(16 * 4096);
  if(p == (char*)-1){ printf(1, "sbrk failed\n"); exit(); }
  printf(1, "=== After sbrk(+16 pages) ===\n");
  memstats();

  printf(1, "\nmemtest done\n");
  exit();
}
