# Asg 10 Q1: Tracking Physical Memory Allocation

## Overview
Implements counters in kalloc.c to track total allocated and freed pages, exposed via memstats() syscall.

## Design
- **kalloc.c:** Added static counters total_allocated and total_freed. Incremented in kalloc() and kfree() respectively under kmem.lock. Getters expose them to sysproc.c.
- **memstats syscall (22):** Prints total pages managed, currently free, total allocated ever, total freed ever, and currently allocated (allocated - freed).
- **memtest.c:** Allocates 8 pages via sbrk, displays stats, releases them, displays stats again, then allocates 16 pages and displays final stats.

## Observed Results (q1_memtest.png)
- Initial: free=56790, allocated-freed matches kernel boot allocations
- After sbrk(+8): free drops to 56782, currently allocated increases by 8
- After sbrk(-8): free returns to 56790, currently allocated back to initial (no leak)
- After sbrk(+16): free drops to 56774, currently allocated increases by 16

## Key Observations
1. Counters correctly track cumulative allocations and frees
2. sbrk(-n) properly frees pages (free count returns to pre-allocation value)
3. No memory leaks: allocated - freed = currently in use

## Build & Run
```bash
make clean && make qemu-nox
$ memtest
```
