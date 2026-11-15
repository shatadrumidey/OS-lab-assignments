# Asg 3 Q1: First Come First Served (FCFS) Scheduler

## Overview
Replaces xv6 default Round-Robin scheduler with a non-preemptive FCFS scheduler.

## Design
- **proc.h:** Added `int ctime` field to `struct proc` to store creation time.
- **proc.c (allocproc):** Set `p->ctime = ticks` when the process is created.
- **proc.c (scheduler):** Among all RUNNABLE processes, picks the one with the smallest `ctime` (oldest). The running process has state RUNNING (not RUNNABLE), so it is not re-selected until it exits — effectively non-preemptive.

## Test Program (fcfstest.c)
- Forks 3 child processes with staggered creation times (busy-loop delay between forks).
- Each child runs a CPU-bound loop and prints its PID + completion message.
- Parent waits for all children, then prints the expected order.

## Observed Results (q1_fcfs.png)
- Process 1 finishes first, then Process 2, then Process 3.
- Completion order strictly matches creation order — FCFS verified.

## Build & Run
```
make clean && make qemu-nox
$ fcfstest
```
