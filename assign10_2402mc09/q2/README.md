# Asg 10 Q2: Virtual-to-Physical Address Translation

## Overview
Implements vtop() syscall to translate virtual addresses to physical addresses using walkpgdir().

## Design
- **vm.c:** vtop(uint va) walks page table, extracts PTE, combines frame + offset.
- **Returns:** Physical address if valid, -1 if unmapped/invalid.
- **vttest.c:** Allocates 4 pages, tests valid VAs and invalid ones (0xFFFFFFFF, NULL).

## Observed Results (q2_vttest.png)
- Valid pages: Shows correct physical addresses
- Invalid addresses: Returns -1 (unmapped/invalid)

## Build & Run
```bash
make clean && make qemu-nox
$ vttest
```
