# Assignment 9: Virtual Memory Management in xv6

**Name:** Shatadrumi Dey | **Roll No:** 2402MC09

---

## Overview

This repository implements three virtual memory management experiments in xv6:

| Question | Topic | Key Implementation |
| :--- | :--- | :--- |
| **Q1** | Page Table Print | `vmprint` syscall walks the page directory and prints user mappings |
| **Q2** | Lazy Allocation (Page Faults) | `T_PGFLT` trap handler + `lazyalloc` for demand paging |
| **Q3** | Page Replacement (FIFO/LRU) | User-space simulation comparing FIFO and LRU on a reference string |

---

## Consolidated Report

### Q1: Printing the Page Table of a Process (`vmprint`)

**Design:** Implemented `vmprint(pde_t *pgdir)` in `vm.c` as a new syscall (22). It walks the two-level page table of the calling process, skipping PDEs at or above `KERNBASE` (kernel mappings). For each valid user PTE it prints the virtual address, physical frame address, and flags (P, W, U). At the end it reports the total number of valid user pages and the number of page-table pages used.

**Test (`pttest.c`):** Calls `vmprint()` at startup and again after `sbrk(4 * 4096)`.

**Observed Results:**
- Startup: 3 user pages (text, guard, stack), 1 page-table page.
- After `sbrk(4*4096)`: exactly 4 new PTEs appear, total = 7 user pages, still 1 page-table page (all fit in one PT).
- The stack guard page is correctly identified (`PTE_U` cleared).

---

### Q2: Handling Page Faults (Lazy Allocation)

**Design:** Modified `sys_sbrk()` so that for `n > 0` it only increments `cur->sz` without calling `growproc()` (no physical allocation). Added a `T_PGFLT` case in `trap.c` that calls `lazyalloc(rcr2())`: validates `va < sz`, rejects guard pages, allocates a zeroed frame via `kalloc()`, maps it with `PTE_W|PTE_U`, and increments a new `cur->nfaults` counter. Modified `copyuvm()` and `deallocuvm()` in `vm.c` to skip absent PTEs (`continue` instead of `panic`) so `fork()` works with lazy regions. Added `getfaults()` syscall (23) to expose the fault count.

**Test (`faulttest.c`):** Calls `sbrk(32*4096)`, writes to exactly 5 pages, prints fault count, then forks a child that touches `va == sz`.

**Observed Results:**
- `sbrk(32 pages)` succeeds with **0 faults** (lazy).
- After touching 5 pages: **exactly 5 faults**, read-back shows `A B C D E`.
- Child accessing `va == sz` is killed with `pid N: invalid page fault at va=X - kill process`; parent survives.
- `getfaults()` correctly returns the handled fault count.

---

### Q3: Page Replacement Algorithms (FIFO and LRU)

**Design:** Pure user-space program (no kernel changes). Implements FIFO (circular replacement pointer) and LRU (tracks `last_use[i]` per frame) on a fixed reference string `7 0 1 2 0 3 0 4 2 3 0 3 2 1 2 0 1 7 0 1` (20 references). Prints frame contents and hit/fault status for every reference, then a summary table for 3, 4, and 5 frames.

**Observed Results:**

| Frames | FIFO Faults | LRU Faults | FIFO Ratio | LRU Ratio |
| :---: | :---: | :---: | :---: | :---: |
| 3 | 15 | 12 | 0.25 | 0.40 |
| 4 | 10 | 8 | 0.50 | 0.60 |
| 5 | 8 | 7 | 0.60 | 0.65 |

Matches expected values (15 FIFO, 12 LRU for 3 frames). LRU consistently outperforms FIFO by exploiting locality of reference.

---

## Directory Structure

```
Asg9_Submission/
── README.md                 # This file
├── q1/
│   ├── README.md
│   ├── pttest.c
│   ├── vm.c, sysproc.c, syscall.*, defs.h, user.h, usys.S, Makefile
│   ├── diffs/
│   └── q1_pttest.png
── q2/
│   ├── README.md
│   ├── faulttest.c
│   ├── vm.c, trap.c, sysproc.c, proc.h, syscall.*, defs.h, user.h, usys.S, Makefile
│   ├── diffs/
│   └── q2_faulttest.png
└── q3/
    ├── README.md
    ├── pagerepl.c
    ├── Makefile
    ├── q3_pagerepl.c.diff, q3_Makefile.diff
    └── q3_pagerepl.png
```

---

## Build & Run

For each question, work in the corresponding `asg9/qX-<name>` folder:

```bash
cd ../asg9/qX-<folder>
make clean && make qemu-nox
$ pttest       # Q1
$ faulttest    # Q2
$ pagerepl     # Q3 (default 3 frames)
$ pagerepl 4   # Q3 with 4 frames
$ pagerepl 5   # Q3 with 5 frames
```

## Key Takeaways

1. **Q1** exposes the page table to user space, proving `fork()` creates separate physical pages for the same virtual address.
2. **Q2** demonstrates demand paging: `sbrk(32 pages)` consumes 0 frames until pages are touched, and invalid accesses kill only the offending process.
3. **Q3** confirms LRU outperforms FIFO by exploiting temporal locality, with faults monotonically decreasing as frames increase.
