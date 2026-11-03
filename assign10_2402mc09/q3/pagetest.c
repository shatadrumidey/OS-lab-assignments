#include "types.h"
#include "stat.h"
#include "user.h"

int
main(void)
{
  char *p;
  int va;

  printf(1, "=== Page Table Management and Protection Test ===\n\n");

  // Allocate a page
  printf(1, "Step 1: Allocating a page via sbrk...\n");
  p = sbrk(4096);
  if(p == (char*)-1){
    printf(1, "sbrk failed\n");
    exit();
  }
  
  va = (int)p;
  printf(1, "Allocated page at virtual address: %x\n\n", va);

  // Touch the page to ensure it's allocated
  p[0] = 'A';
  printf(1, "Step 2: Writing to page (touching it)...\n");
  printf(1, "  Wrote 'A' to page[0]\n\n");

  // Display page info
  printf(1, "Step 3: Getting page information...\n");
  pageinfo(va);
  printf(1, "\n");

  // Set page to read-only
  printf(1, "Step 4: Setting page to read-only...\n");
  setreadonly(va);
  printf(1, "\n");

  // Verify the change
  printf(1, "Step 5: Verifying page is now read-only...\n");
  pageinfo(va);
  printf(1, "\n");

  // Try to write to read-only page (should cause page fault)
  printf(1, "Step 6: Attempting to write to read-only page...\n");
  printf(1, "  (This should trigger a page fault and kill the process)\n");
  
  // This write should fail with a page fault
  p[0] = 'B';
  
  // If we reach here, the write succeeded (which shouldn't happen)
  printf(1, "  ERROR: Write succeeded (should have faulted!)\n");
  printf(1, "  Read value: %c\n", p[0]);
  
  printf(1, "\n=== pagetest done ===\n");
  exit();
}
