Asg 9 Q1: Printing the Page Table of a Process (xv6)

KERNEL SIDE (vm.c, syscall 22)
vmprint(pgdir) walks both levels of the calling process's page table:
- Skips PDEs at/above KERNBASE (kernel mappings ignored).
- For each present user PDE: prints the page-table page count, then for
  each present PTE prints VA, physical frame address, and flags P, W, U.
- At the end: prints total valid user pages and number of page-table pages
  (one per present user PDE).

USER SIDE (pttest.c) AND OBSERVED LOG (q1_pttest.png)
1) at startup: PDE 0 with PTEs for text/data/stack; total user pages = N,
   page-table pages = 1.
2) after sbrk(4*4096): exactly 4 new PTEs appear under PDE 0; total user
   pages = N+4, page-table pages still 1 (all fit in one PT page).

BUILD & RUN: make clean && make qemu-nox ; $ pttest
