Asg 8 Q4: Lazy Memory Allocation / Demand Paging (xv6)

DESIGN
Instead of allocating physical frames at sbrk time (eager allocation), this
implementation reserves virtual address space (increments cur->sz) but
defers frame allocation until the process actually accesses the page via a
page fault (demand paging).

sys_sbrk (sysproc.c):
- n > 0: only increments cur->sz (no growproc call, no kalloc)
- n < 0: still calls deallocuvm to free actually-allocated pages
- Returns the old sz (base address of the new region)

trap.c (case T_PGFLT):
- Reads faulting address from cr2 via rcr2()
- Calls lazyalloc(va): validates va < sz, rejects guard pages (PTE_P but
  PTE_U cleared), allocates a zeroed frame, maps it with mappages(PTE_W|PTE_U)
- Returns from trap, CPU retries the faulting instruction (now succeeds)
- Invalid faults (va >= sz, guard page, kalloc failure): print message,
  set cur->killed = 1, only the offending process dies (kernel survives)

vm.c (lazyalloc + edge cases):
- lazyalloc(va): walks page table, allocates frame, maps PTE_W|PTE_U
- copyuvm: skips not-present PTEs (continue instead of panic) so fork of
  a process with lazy regions works (child inherits absent PTEs)
- deallocuvm: skips not-present PTEs (nothing to free)
- freevm: same as deallocuvm, plus early panic if pgdir == 0
- copyin/copyout: fault in lazy pages instead of returning -1 (so
  read()/write() with untouched buffers work)

lazytest.c AND OBSERVED LOG (q4_lazytest.png)
[before 32MB sbrk] free=56790, user_pages=3, pt=66
[after 32MB sbrk]  free=56790 (UNCHANGED!), user_pages=8195, pt=66
                   -> 32 MB reserved in virtual space, zero frames consumed
[after touching 3 pages (at offsets 0, 16MB, 32MB-1)]
                   free=56785, pt=68
                   -> only 5 frames allocated (3 pages + 2 page-table pages)
read back: A B C   -> faulted-in pages hold correct data
write() on untouched buffer (ok)
                   -> kernel copyin faults in the page, writes 8 zero bytes
child sees touched value: B (copyuvm skipped absent PTEs)
                   -> fork works despite 32 MB of lazy regions
[after fork+reap]  free=56784, user_pages=8195, pt=66
                   -> child's pages freed (parent's write()-faulted page
                      remains allocated in parent)
child touching va == sz (invalid)...
pid 5: invalid page fault at va=2003000 - kill process
parent + kernel alive after child was killed for bad access
lazytest done

CONCLUSION
Large sbrk (32 MB) succeeds immediately without consuming physical memory
(free_pages unchanged). Only touched pages consume frames (demand paging).
Invalid accesses (va >= sz) terminate only the offending process; kernel
and other processes continue. Fork of a process with lazy regions works
(copyuvm skips absent PTEs). read()/write() with untouched buffers work
(kernel copyin faults in pages).

BUILD & RUN: make clean && make qemu-nox ; $ lazytest
