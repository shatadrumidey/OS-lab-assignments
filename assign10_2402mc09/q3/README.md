# Asg 10 Q3: Page Table Management and Page Protection

## Overview
Implements pageinfo() and setreadonly() syscalls to inspect and modify page permissions.

## Design
- **pageinfo(uint va):** Walks page table, displays physical frame and flags (P, W, U), shows permissions.
- **setreadonly(uint va):** Removes PTE_W flag from page table entry, making page read-only.
- **pagetest.c:** Allocates page, displays info, sets read-only, attempts write (triggers page fault).

## Observed Results (q3_pagetest.png)
- Initial page: P=1, W=1, U=1 (Read-Write)
- After setreadonly: P=1, W=0, U=1 (Read-Only)
- Write attempt: Page fault occurs, process killed by kernel

## Build & Run
```bash
make clean && make qemu-nox
$ pagetest
```
