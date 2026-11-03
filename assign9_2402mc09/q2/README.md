# Asg 9 Q2: Handling Page Faults (Lazy Allocation)

## Overview
Implements demand paging for xv6 heap. sbrk() only grows cur->sz; frames allocated on first access via T_PGFLT trap.

## Design
- sys_sbrk: n>0 only increments cur->sz (no growproc)
- lazyalloc: validates va<sz, rejects guard pages, allocates zeroed frame, maps PTE_W|PTE_U, increments nfaults
- trap.c T_PGFLT: calls lazyalloc(rcr2()), kills process on failure
- copyuvm/deallocuvm: skip absent PTEs (continue instead of panic)
- struct proc: new field int nfaults
- getfaults syscall (23): returns myproc()->nfaults

## Test: faulttest.c
1. sbrk(32*4096) -> 0 faults
2. Write 5 pages -> 5 faults, read-back A B C D E
3. Fork child touches va==sz -> child killed, parent survives

## Observed Output
sbrk(32 pages) succeeded, faults so far: 0
touched 5 pages, faults now: 5 (expect 5)
read back: A B C D E
child touching va == sz (invalid)...
pid N: invalid page fault at va=... - kill process
parent alive after child killed for bad access
faulttest done

## Build
make clean && make qemu-nox; $ faulttest
