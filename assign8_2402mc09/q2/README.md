Asg 8 Q2: Physical memory statistics (xv6 kalloc instrumentation)

KERNEL SIDE (kalloc.c, vm.c, sysproc.c)
- kmem gains two counters, updated under kmem.lock:
  free_pages  : decremented in kalloc(), incremented in kfree();
  total_pages : captured once at the end of kinit2(), when the full free
                list has been built (57001 pages on this build).
- getters nfree_pages()/ntotal_pages() read them under the lock.
- vm.c count_pt_pages(pgdir) = 1 (page-directory page) + one per present
  PDE (each present PDE points to a page-table page). xv6 maps the kernel
  region into every process pgdir, so even a tiny process shows ~66
  page-table pages - expected, not a leak.
- meminfo.h: struct meminfo { total_pages, free_pages, user_pages,
  pt_pages }, include-guarded, shared by kernel and user code.
- syscall getmeminfo(mi) = 22: argptr() validates the user pointer, then
  fills total, free, cur->sz/PGSIZE and count_pt_pages(cur->pgdir).

USER SIDE (memtest.c) AND OBSERVED LOG (q2_memtest.png)
[initial]             total=57001 free=56790 user=3  pt=66
[after sbrk +8 pages] total=57001 free=56782 user=11 pt=66
                      -> free fell by exactly 8, user_pages rose by 8
[parent/child alive]  free=56704
                      -> fork cost 78 pages = 11 copied user pages + the
                         child's own page-table pages (pgdir + kernel
                         region PTs + user PTs)
[after child reaped]  free=56782  == the pre-fork value
free before fork=56782, after reap=56782 -> NO LEAK (equal)
(The parent and child print concurrently, so those two lines interleave
on the console - harmless.)

CONCLUSION
The free-page count decreases on allocation (sbrk) and on fork, and returns
exactly to its pre-fork value after the child exits and is reaped: xv6's
freevm/kfree path leaks nothing.

BUILD & RUN: make clean && make qemu-nox ; $ memtest
