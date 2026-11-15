# Asg 4 Q1: Shortest Remaining Time First (SRTF) Scheduler

**Student:** Shaan Shan | **Roll No:** 2402MC08 | **Course:** MA3105

## Overview
Preemptive version of SJF. The scheduler always runs the process with the smallest remaining burst, and a newly arrived shorter job preempts the running one.

## Design
- **proc.h:** added `int burst_time` and `int remaining_time` to `struct proc`.
- **proc.c (allocproc):** both fields initialised to 0 (unset).
- **proc.c (set_burst):** kernel helper that sets `burst_time` and seeds `remaining_time` with the same value.
- **sysproc.c (sys_set_burst):** syscall wrapper reading (pid, bt) via argint.
- **proc.c (scheduler):** among RUNNABLE processes picks the smallest `remaining_time`. A value of 0 or less is treated as 999999 so undeclared processes run last.
- **trap.c (timer interrupt):** decrements `remaining_time` of the running process each tick, then scans the table under `ptable.lock`. If any RUNNABLE process has a strictly smaller `remaining_time`, it calls `yield()` to preempt.

## Test Program (srtftest.c)
- Forks 3 processes with staggered arrivals: P1 burst 100, then P2 burst 10, then P3 burst 5.
- Each child calls `set_burst(getpid(), bt)` before its CPU-bound loop.
- P1 starts first, then gets preempted when the shorter jobs arrive.

## Observed Results (q1_srtf.png)
- The short jobs (P3, P2) finish before the long job P1 completes.
- Confirms preemption triggers correctly on arrival of a shorter job.

## Build and Run
```bash
cp -r xv6-clean asg4_test && cd asg4_test
cp ../Asg4_Submission/q1/* .
sed -i "s/-Werror//g" Makefile
make clean && make qemu-nox
```
At the xv6 prompt: `srtftest`. Exit QEMU with Ctrl+A then X.
