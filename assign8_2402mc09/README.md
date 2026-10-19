# Assignment 8: Virtual Memory Management in xv6
**Nme:** Shatadrumi Dey | **Roll No:** 2402MC09 

## Overview
This repository implements four virtual memory extensions in xv6:
1. **Q1 (VA to PA):** `sys_va2pa` syscall exposing page table walks.
2. **Q2 (Memory Stats):** `getmeminfo` syscall tracking free/total pages.
3. **Q3 (Page Table Dump):** `vmprint` syscall for hierarchical dumps.
4. **Q4 (Lazy Allocation):** Demand paging via `T_PGFLT` trap handler.

## Consolidated Report
### Q1: Virtual-to-Physical Translation
Exposed `walkpgdir` via syscall 22. Prints PDX, PTX, offset, and flags. Proves `fork()` creates separate physical pages for the same virtual address.

### Q2: Physical Memory Statistics
Instrumented `kalloc.c` to track `free_pages`. `sbrk` drops free pages, `fork` drops them further, and `exit`+`wait` restores the exact pre-fork count (no leaks).

### Q3: Page Table Dump
`vmprint` iterates PDEs. User PDEs are dumped with indented PTEs. Kernel PDEs are summarized. Identifies the stack guard page (`PTE_U` cleared).

### Q4: Lazy Allocation
`sys_sbrk` only grows `sz`. `trap.c` handles `T_PGFLT` by calling `lazyalloc` to map a zeroed frame. `copyuvm` skips absent PTEs. 32MB sbrk consumes 0 frames until touched.

## Directory Structure
- `q1/`: `vatest.c`, diffs, and screenshot.
- `q2/`: `memtest.c`, `meminfo.h`, diffs, and screenshot.
- `q3/`: `vmprinttest.c`, diffs, and screenshot.
- `q4/`: `lazytest.c`, `meminfo.h`, diffs, and screenshot.
