#include "types.h"
#include "stat.h"
#include "user.h"

int
main(void)
{
  char *p;
  int va1, va2, va3, va_invalid;
  int pa1, pa2, pa3, pa_invalid;

  printf(1, "=== Virtual to Physical Address Translation Test ===\n\n");

  // Allocate some memory
  printf(1, "Allocating 4 pages via sbrk...\n");
  p = sbrk(4 * 4096);
  if(p == (char*)-1){
    printf(1, "sbrk failed\n");
    exit();
  }
  
  // Touch the pages to ensure they're allocated
  p[0] = 'A';
  p[4096] = 'B';
  p[8192] = 'C';
  p[12288] = 'D';
  
  printf(1, "Memory allocated at virtual address: %x\n\n", (uint)p);

  // Test 1: First page
  va1 = (int)p;
  pa1 = vtop(va1);
  printf(1, "Test 1 - First page:\n");
  printf(1, "  Virtual Address:  %x\n", va1);
  if(pa1 == -1)
    printf(1, "  Physical Address: unmapped/invalid\n");
  else
    printf(1, "  Physical Address: %x\n", pa1);
  printf(1, "\n");

  // Test 2: Second page
  va2 = (int)(p + 4096);
  pa2 = vtop(va2);
  printf(1, "Test 2 - Second page:\n");
  printf(1, "  Virtual Address:  %x\n", va2);
  if(pa2 == -1)
    printf(1, "  Physical Address: unmapped/invalid\n");
  else
    printf(1, "  Physical Address: %x\n", pa2);
  printf(1, "\n");

  // Test 3: Third page
  va3 = (int)(p + 8192);
  pa3 = vtop(va3);
  printf(1, "Test 3 - Third page:\n");
  printf(1, "  Virtual Address:  %x\n", va3);
  if(pa3 == -1)
    printf(1, "  Physical Address: unmapped/invalid\n");
  else
    printf(1, "  Physical Address: %x\n", pa3);
  printf(1, "\n");

  // Test 4: Invalid address (way beyond allocated space)
  va_invalid = (int)0xFFFFFFFF;
  pa_invalid = vtop(va_invalid);
  printf(1, "Test 4 - Invalid address (0xFFFFFFFF):\n");
  printf(1, "  Virtual Address:  %x\n", va_invalid);
  if(pa_invalid == -1)
    printf(1, "  Physical Address: unmapped/invalid (as expected)\n");
  else
    printf(1, "  Physical Address: %x\n", pa_invalid);
  printf(1, "\n");

  // Test 5: NULL address
  printf(1, "Test 5 - NULL address (0x0):\n");
  printf(1, "  Virtual Address:  0\n");
  pa_invalid = vtop(0);
  if(pa_invalid == -1)
    printf(1, "  Physical Address: unmapped/invalid (as expected)\n");
  else
    printf(1, "  Physical Address: %x\n", pa_invalid);
  printf(1, "\n");

  printf(1, "=== vttest done ===\n");
  exit();
}
