# Asg 3 Q3: Shortest Job First (SJF) Scheduler

## Overview
Non-preemptive SJF scheduler using estimated burst time provided by processes.

## Design
- **proc.h:** Added `int burst_time` to struct proc (default 0).
- **proc.c (allocproc):** Initialize burst_time = 0.
- **proc.c (scheduler):** Picks RUNNABLE process with smallest burst_time. burst_time <= 0 treated as 999999 (lowest priority).
- **proc.c (set_burst):** Kernel helper to set burst time.
- **sysproc.c (sys_set_burst):** Syscall wrapper.

## Test Program (sjftest.c)
- Forks 4 procs, each calls set_burst(getpid(), X) with X=40,10,30,20.
- Each runs equal CPU-bound loop.

## Observed Results (q3_sjf.png)
- Completion order: P2(10), P4(20), P3(30), P1(40).
- Processes complete in ascending order of declared burst time.

## Build & Run
```
make clean && make qemu-nox
$ sjftest
```
