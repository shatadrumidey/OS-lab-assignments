Asg 8 Q3: Dumping the page table of a process (xv6)

KERNEL SIDE (vm.c, syscall 22)
vmprint(pgdir) walks both levels of the calling process's page table:
- Level 1: for every present PDE it prints the index, the physical address
  of the page table (PTE_ADDR) and its flags.
- Level 2 (indented): for every present PTE below KERNBASE it prints the
  entry index, the virtual address ((pdx<<22)|(ptx<<12)), the frame physical
  address and flags P, W, U. PDEs at/above KERNBASE are only counted, and a
  one-line summary reports how many kernel entries exist.
- The stack guard page created by exec() (clearpteu clears PTE_U on the
  page at sz-2*PGSIZE) is mapped but not user-accessible, so it is tagged
  "<== guard page (mapped, U cleared)".
- A final check line verifies: user-accessible page count ==
  sz/PGSIZE - 1 (guard page excluded) -> MATCH.

USER SIDE (vmprinttest.c) AND OBSERVED LOG (q3_vmprinttest.png)
1) at startup: PDE 0 with PTEs for text/data/heap/stack + the tagged guard
   page; kernel PDEs (512+) summarized, not dumped; check line MATCH.
2) after sbrk(8*4096): eight new heap PTEs appear under PDE 0.
3) after sbrk(-4096): the last heap PTE disappears (deallocuvm freed it).
The dump is consistent at every step: hierarchy, indentation and counts.

BUILD & RUN: make clean && make qemu-nox ; $ vmprinttest
