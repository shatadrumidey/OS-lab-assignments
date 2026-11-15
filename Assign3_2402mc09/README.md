# Assignment 3: CPU Scheduling in xv6

**Student:** Shatadrumi Dey | **Roll No:** 2402MC09 | **Course:** MA3105 (Operating Systems)

---

## Overview

Three schedulers implemented by replacing the default Round-Robin scheduler in xv6.

| Q | Scheduler | Type | Key Field | Selection Rule |
| :--- | :--- | :--- | :--- | :--- |
| 1 | FCFS | Non-preemptive | `ctime` | Smallest creation time |
| 2 | Priority + Aging | Preemptive | `priority`, `wait_ticks` | Lowest priority number, boost after 30 ticks |
| 3 | SJF | Non-preemptive | `burst_time` | Smallest declared burst time |

---

## Q1: First Come First Served (FCFS)

**Files modified:** `proc.h`, `proc.c`, `Makefile` | **Test:** `fcfstest.c`

- Added `int ctime` to `struct proc`.
- In `allocproc()`: `p->ctime = ticks` records arrival time.
- `scheduler()` scans the process table and selects the RUNNABLE process with the smallest `ctime`.
- Non-preemptive behaviour: a RUNNING process is not in the RUNNABLE pool, so it is never re-selected mid-execution; the next process is chosen only after it exits or blocks.

**Result:** 3 staggered processes complete strictly in creation order (P1 then P2 then P3).

---

## Q2: Priority-Based Preemptive Scheduler with Aging

**Files modified:** `proc.h`, `proc.c`, `sysproc.c`, `syscall.c`, `syscall.h`, `defs.h`, `user.h`, `usys.S`, `Makefile` | **Test:** `priotest.c`

- Added `int priority` (1 = highest, 20 = lowest, default 10) and `int wait_ticks`.
- New syscall `set_priority(pid, priority)` validates the range 1..20 and updates the target process.
- `scheduler()` picks the lowest priority number; ties are broken by the larger `wait_ticks` (round-robin fairness).
- **Aging:** every scheduling pass increments `wait_ticks` of each RUNNABLE process. Once it crosses 30, `priority` is decremented by 1 (raised) and the counter resets. This guarantees no process starves.
- **Preemption:** the timer interrupt yields the CPU each tick, so a newly runnable higher-priority process takes over promptly.

**Result:** processes with priorities 1, 5, 10, 20 finish in that order; the lowest-priority process still completes because aging eventually raises it.

---

## Q3: Shortest Job First (SJF) with Estimated Burst Time

**Files modified:** `proc.h`, `proc.c`, `sysproc.c`, `syscall.c`, `syscall.h`, `defs.h`, `user.h`, `usys.S`, `Makefile` | **Test:** `sjftest.c`

- Added `int burst_time` to `struct proc`, initialised to 0 (unset).
- New syscall `set_burst(pid, bt)` lets a process declare its expected CPU burst before heavy computation.
- `scheduler()` selects the RUNNABLE process with the smallest `burst_time`. A value of 0 or less is treated as 999999, so undeclared processes are scheduled last.
- Non-preemptive: the selected process runs until it exits or blocks.

**Result:** processes declaring bursts 40, 10, 30, 20 complete in ascending burst order (10, 20, 30, 40).

---

## Comparative Write-up

### Turnaround time: CPU-bound workloads

| Scheduler | Behaviour on CPU-bound batch | Avg turnaround |
| :--- | :--- | :--- |
| FCFS | Convoy effect: one long job delays every job behind it | Worst |
| Priority + Aging | Important jobs finish early, but ordering is user-declared, not length-aware | Medium |
| SJF | Shortest jobs cleared first, provably minimal average waiting time when all jobs are available together | Best |

**Why:** SJF minimises the sum of completion times because each short job removed early stops contributing to the waiting time of all remaining jobs. FCFS does the opposite: a long job at the head inflates the waiting time of every follower. Priority scheduling only helps if the assigned priorities happen to correlate with job length.

### Turnaround time: mixed (CPU + I/O) workloads

| Scheduler | Behaviour on mixed workload | Verdict |
| :--- | :--- | :--- |
| FCFS | I/O-bound jobs queue behind long CPU bursts; devices sit idle | Poor |
| Priority + Aging | I/O-bound jobs can be given high priority so they run, issue I/O, and block quickly; aging protects long CPU jobs | Best |
| SJF | Works only if burst estimates are accurate; a wrong estimate misorders the queue and long jobs can starve (no aging) | Fragile |

**Why:** in a mixed workload the goal is to keep I/O devices busy, which means letting short interactive jobs reach their blocking call fast. Priority scheduling achieves this directly and aging removes the starvation risk. SJF depends on honest burst estimates from user space, which is unrealistic for interactive processes whose next burst is unpredictable.

### Summary

- **FCFS** — simplest, zero bookkeeping, no starvation, but worst average turnaround (convoy effect).
- **Priority + Aging** — best all-round choice for mixed workloads; preemption gives responsiveness and aging gives fairness. Cost: per-tick aging bookkeeping.
- **SJF** — best average turnaround for CPU-bound batches with known lengths, but needs cooperative burst declaration and has no starvation guard.

---

## Directory Structure

```
Asg3_Submission/
├── README.md              # this file (consolidated report + comparison)
├── q1/                    # FCFS
│   ├── README.md, fcfstest.c, proc.c, proc.h, Makefile
│   ├── diffs/             # unified diffs vs pristine xv6-public
│   └── q1_fcfs.png
├── q2/                    # Priority + Aging
│   ├── README.md, priotest.c, proc.c, proc.h, sysproc.c,
│   │   syscall.c, syscall.h, defs.h, user.h, usys.S, Makefile
│   ├── diffs/
│   └── q2_priority.png
└── q3/                    # SJF
    ├── README.md, sjftest.c, proc.c, proc.h, sysproc.c,
    │   syscall.c, syscall.h, defs.h, user.h, usys.S, Makefile
    ├── diffs/
    └── q3_sjf.png
```

---

## Build and Run

Each question is a separate xv6 tree. Copy pristine xv6, overlay the question files, then build.

```bash
cp -r xv6-clean asg3_test && cd asg3_test
cp ../Asg3_Submission/q1/* .        # or q2/ or q3/
sed -i "s/-Werror//g" Makefile      # required on modern GCC
make clean && make qemu-nox
```

Then at the xv6 shell prompt:

```
$ fcfstest      # Q1
$ priotest      # Q2
$ sjftest       # Q3
```

Exit QEMU with Ctrl+A then X.

**Note:** `-Werror` must be removed because modern GCC flags the intentional recursion in the stock `sh.c` as an error, which aborts the build.
