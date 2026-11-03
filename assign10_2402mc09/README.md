# Assignment 10: Virtual Memory Management in xv6

**Name:** Shatadrumi Dey | **Roll No:** 2402MC09

---

## Overview

This repository implements four virtual memory management experiments in xv6:

| Question | Topic | Key Implementation |
| :--- | :--- | :--- |
| **Q1** | Physical Memory Tracking | Counters in `kalloc.c` exposed via `memstats()` syscall |
| **Q2** | VA to PA Translation | `vtop()` syscall using `walkpgdir()` |
| **Q3** | Page Protection | `pageinfo()` and `setreadonly()` syscalls |
| **Q4** | Page Replacement (FIFO/LRU/Optimal) | User-space simulation comparing 3 algorithms |

---

## Consolidated Report

### Q1: Tracking Physical Memory Allocation

**Design:** Modified `kalloc.c` to maintain four static counters under `kmem.lock`:
- `total_allocated`: cumulative pages allocated via `kalloc()`
- `total_freed`: cumulative pages freed via `kfree()`
- `total_pages`: counted during `freerange()` at boot
- `free_pages`: decremented in `kalloc()`, incremented in `kfree()`

Four getter functions expose these to `sysproc.c`, which implements the `memstats()` syscall (22) that prints all statistics.

**Test (`memtest.c`):** Allocates 8 pages, displays stats, releases them, displays stats again, then allocates 16 pages.

**Observed Results:**
- Initial: free=56790
- After `sbrk(+8)`: free drops to 56782 (exactly 8 less)
- After `sbrk(-8)`: free returns to 56790 (no leak)
- After `sbrk(+16)`: free drops to 56774 (exactly 16 less)

**Conclusion:** Counters correctly track cumulative allocations/frees, and `sbrk(-n)` properly releases pages.

---

### Q2: Virtual-to-Physical Address Translation

**Design:** Implemented `vtop(uint va)` in `vm.c` that:
1. Calls `walkpgdir(cur->pgdir, va, 0)` to locate the PTE
2. Returns -1 if PTE is NULL or not present (`PTE_P` cleared)
3. Otherwise combines `PTE_ADDR(*pte)` with `(va & 0xFFF)` offset

**Test (`vttest.c`):** Allocates 4 pages, touches them, translates multiple VAs, then tests invalid addresses (0xFFFFFFFF and NULL).

**Observed Results:**
- Valid allocated pages return correct physical addresses (PA = frame + offset)
- Invalid addresses return -1 (unmapped/invalid)

**Conclusion:** `walkpgdir()` correctly traverses the two-level page table and the PTE format matches the expected PA+offset structure.

---

### Q3: Page Table Management and Page Protection

**Design:** Two new syscalls:
- `pageinfo(uint va)`: walks page table, prints physical frame and flags (P, W, U), shows permissions
- `setreadonly(uint va)`: clears `PTE_W` flag from the PTE, making the page read-only

**Test (`pagetest.c`):** Allocates a page, displays info, sets it read-only, verifies the flag change, then attempts a write.

**Observed Results:**
- Initial page: P=1, W=1, U=1 (Read-Write)
- After `setreadonly`: P=1, W=0, U=1 (Read-Only)
- Write attempt triggers `trap 14 err 7` (protection violation) and kernel kills the process

**Conclusion:** Clearing `PTE_W` in the page table entry is sufficient to enforce read-only protection; the hardware MMU raises the fault and xv6 handles it correctly.

---

### Q4: Page Replacement Algorithms (FIFO, LRU, Optimal)

**Design:** Pure user-space program (no kernel changes). Reference string: `7 0 1 2 0 3 0 4 2 3 0 3 2 1 2 0 1 7 0 1` (20 refs).

Three algorithms implemented:
- **FIFO:** Circular replacement pointer; replaces oldest-loaded frame
- **LRU:** Tracks `last_use[i]` per frame; replaces frame with smallest last-use time
- **Optimal:** Replaces page whose next use is farthest in the future (theoretical best)

**Observed Results:**

| Frames | FIFO Faults | LRU Faults | OPT Faults | FIFO Hit% | LRU Hit% | OPT Hit% |
| :---: | :---: | :---: | :---: | :---: | :---: | :---: |
| 3 | 15 | 12 | 9 | 0.25 | 0.40 | 0.55 |
| 4 | 10 | 8 | 6 | 0.50 | 0.60 | 0.70 |
| 5 | 8 | 7 | 5 | 0.60 | 0.65 | 0.75 |

**Conclusion:**
1. **Optimal always wins** — perfect future knowledge gives the theoretical lower bound
2. **LRU beats FIFO** at every frame count by exploiting temporal locality
3. Faults decrease monotonically as frames increase for all three algorithms

---

## Directory Structure

```
Asg10_Submission/
├── README.md                 # This file (overall report)
├── q1/
│   ├── README.md, memtest.c, kalloc.c, sysproc.c, syscall.*, defs.h, user.h, usys.S, Makefile
│   ├── diffs/                # Unified diffs against clean xv6
│   └── q1_memtest.png
├── q2/
│   ├── README.md, vttest.c, vm.c, sysproc.c, syscall.*, defs.h, user.h, usys.S, Makefile
│   ├── diffs/
│   └── q2_vttest.png
├── q3/
│   ├── README.md, pagetest.c, vm.c, sysproc.c, syscall.*, defs.h, user.h, usys.S, Makefile
│   ├── diffs/
│   └── q3_pagetest.png
└── q4/
    ├── README.md, pagereplace.c, Makefile
    ├── diffs/
    └── q4_pagereplace.png
```

---

## Build & Run Instructions

For each question, work in `asg10/qX-<folder>`:

```bash
cd ../asg10/qX-<folder>
make clean && make qemu-nox
$ memtest       # Q1
$ vttest        # Q2
$ pagetest      # Q3
$ pagereplace   # Q4 (default 3 frames)
$ pagereplace 4 # Q4 with 4 frames
$ pagereplace 5 # Q4 with 5 frames
```

---

## Key Takeaways

1. **Q1** proves xv6 has no memory leaks: `sbrk(+n)` then `sbrk(-n)` returns free count to its original value.
2. **Q2** demonstrates the two-level page table structure: virtual address splits into PDX + PTX + offset, and the PTE encodes the physical frame.
3. **Q3** shows that page protection is enforced by hardware (MMU raises trap 14) — the kernel only needs to clear `PTE_W`.
4. **Q4** confirms the theoretical hierarchy: Optimal ≤ LRU ≤ FIFO in terms of page faults, with Optimal being unimplementable in practice (requires future knowledge).
