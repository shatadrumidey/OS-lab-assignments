# Asg 3 Q2: Priority-Based Preemptive Scheduler with Aging

## Overview
Preemptive priority scheduler (1=highest, 20=lowest) with aging to prevent starvation.

## Design
- **proc.h:** Added `int priority` (default 10) and `int wait_ticks` to struct proc.
- **proc.c (allocproc):** Initialize priority=10, wait_ticks=0.
- **proc.c (scheduler):** Each tick, RUNNABLE procs get wait_ticks++. If wait_ticks>30, priority-- (boost) and reset. Then pick lowest priority number; ties broken by larger wait_ticks (round-robin).
- **proc.c (set_proc_priority):** Kernel helper to change a proc priority.
- **sysproc.c (sys_set_priority):** Syscall wrapper reading (pid, priority).
- **Preemption:** Timer interrupt yields every tick, so a higher-priority runnable proc takes over promptly.

## Test Program (priotest.c)
- Forks 4 procs with priorities 1, 5, 10, 20, each doing equal CPU-bound work.
- Each calls set_priority(getpid(), p) then runs a busy loop.

## Observed Results (q2_priority.png)
- Completion order: P1 (prio 1) first, then P2 (5), P3 (10), P4 (20).
- High-priority finishes first; low-priority still finishes (no starvation) thanks to aging.

## Build & Run
```
make clean && make qemu-nox
$ priotest
```
