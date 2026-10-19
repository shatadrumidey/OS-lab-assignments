Asg 8 Q1: Virtual-to-physical translation exposed to user space (xv6)

KERNEL SIDE
vm.c: walkpgdir() made non-static and prototyped in defs.h so the syscall
path can walk the calling process's page table.
sysproc.c sys_va2pa(va): walkpgdir(cur->pgdir, va, 0);
  - returns -1 if the PTE is absent (no PTE_P) or not user-accessible
    (no PTE_U), after a cprintf note;
  - else pa = PTE_ADDR(*pte) + (va & 0xFFF), and cprintf prints PDX(va),
    PTX(va), 12-bit offset, raw PTE and flag bits P, W, U plus the PA
    (task b).
Syscall number 22 (syscall.h/syscall.c/usys.S/user.h).

USER SIDE (vatest.c)
Prints VA and PA (via va2pa) of: a global variable, a local (stack)
variable, and a malloc block; each is accompanied by the kernel's cprintf
line showing PDX/PTX/offset/flags. va2pa(0x7FFFFFFF) returns -1 (unmapped).
fork(): parent and child both translate &globalvar - identical virtual
address, DIFFERENT physical addresses, because fork/copyuvm allocates fresh
physical pages for the child (private copy). Log: q1_vatest.png.

BUILD & RUN: make clean && make qemu-nox ; $ vatest
