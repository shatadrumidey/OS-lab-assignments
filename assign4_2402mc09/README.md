# Assignment 4: Advanced CPU Scheduling in xv6

**Student:** Shatadrumi Dey | **Roll No:** 2402MC09 | **Course:** MA3105 (Operating Systems)

---

## Overview

Three advanced schedulers implemented on top of pristine xv6-public, each replacing the default Round-Robin scheduler.

| Q | Scheduler | Type | Selection Rule | Needs burst estimate |
| :--- | :--- | :--- | :--- | :--- |
| 1 | SRTF | Preemptive | Smallest `remaining_time` | Yes |
| 2 | HRRN | Non-preemptive | Highest `((wait + burst) * 100) / burst` | Yes |
| 3 | MLFQ | Preemptive, 3 queues | Lowest `queue_level`, Round-Robin within a level | No |

---

## Q1: Shortest Remaining Time First (SRTF)

**Test:** `srtftest` | **Files:** `proc.h`, `proc.c`, `trap.c`, syscall wiring, `Makefile`

- Added `burst_time` and `remaining_time` to `struct proc`.
- `set_burst(pid, bt)` stores the estimate and seeds `remaining_time` with the same value.
- `scheduler()` picks the RUNNABLE process with the smallest `remaining_time`; undeclared processes are treated as 999999 and run last.
- The timer interrupt decrements `remaining_time` of the running process, then scans the table under `ptable.lock`. If any RUNNABLE process has a strictly smaller remaining time, it calls `yield()` to preempt.
- **Result:** a long job (burst 100) starts first, then newly arriving short jobs (10 and 5) preempt it and finish before it completes.

---

## Q2: Highest Response Ratio Next (HRRN)

**Test:** `hrrntest` | **Files:** `proc.h`, `proc.c`, `trap.c`, syscall wiring, `Makefile`

- Added `ctime` and `burst_time` to `struct proc`; `ctime` is set from `ticks` in `allocproc()`.
- `set_burst(pid, bt)` declares the expected burst (validated bt >= 1).
- `scheduler()` computes, with pure integer arithmetic, `wait = ticks - ctime` and `rr = ((wait + burst) * 100) / burst`, then selects the highest ratio. If no process declared a burst it falls back to the first RUNNABLE process so init and sh stay usable.
- The timer `yield()` in `trap.c` is disabled, making the scheduler genuinely non-preemptive.
- **Result:** with bursts 300, 15, 10, 20, 25 the completion order is 10, 15, 20, 25, 300. The longest job still completes because its growing wait time lifts its ratio, so starvation is impossible.

---

## Q3: Multilevel Feedback Queue (MLFQ)

**Test:** `mlfqtest` | **Files:** `proc.h`, `proc.c`, `trap.c`, syscall wiring, `Makefile`

- Added `queue_level` (0 = highest, 2 = lowest) and `ticks_used`; every new process starts at level 0.
- `scheduler()` serves level 0 first, then 1, then 2, moving to a lower queue only when the higher one has no RUNNABLE process. A rotating cursor gives Round-Robin inside each level.
- The timer interrupt increments `ticks_used` and compares it with the quantum of the current queue: level 0 = 1 tick, level 1 = 2 ticks, level 2 = 4 ticks. Consuming a full quantum demotes the process (capped at level 2), resets the counter, and yields.
- A voluntary block in `sleep()` resets `ticks_used` and restores level 0, which is the interactive boost that keeps I/O-bound processes at the top.
- `getqlevel(pid)` is a reporting syscall so the test can print live queue levels.
- **Result:** the two CPU-bound children fall to level 2 within a few rounds and stay there, while the two sleep-heavy children remain at level 0 for their whole lifetime.

---

## Comparative Analysis

| Criterion | SRTF | HRRN | MLFQ |
| :--- | :--- | :--- | :--- |
| Preemptive | Yes | No | Yes |
| Requires burst estimate | Yes | Yes | No |
| Starvation protection | None | Yes, ratio rises with wait | Partial, no cross-queue aging |
| Average turnaround (CPU batch) | Best when estimates are accurate | Good, slightly worse than SJF | Depends on workload mix |
| Responsiveness (interactive) | Poor for long jobs | Poor, convoy effect at the head | Best, I/O-bound stay at level 0 |
| Runtime overhead | Per-tick decrement plus table scan | Ratio computation per candidate | Per-tick accounting plus 3-level scan |

### CPU-bound workloads
SRTF gives the lowest average turnaround because every tick of a short job removed early stops adding to the waiting time of all remaining jobs. HRRN is close but non-preemptive, so a long job already at the head still delays everyone behind it. MLFQ does not optimise for turnaround at all: it deliberately pushes CPU-heavy work down, which lengthens the completion time of long jobs.

### Mixed and interactive workloads
MLFQ wins here because it needs no prior knowledge. It infers behaviour from observed CPU usage, so interactive processes that block frequently keep high priority and reach their I/O calls quickly, keeping devices busy. SRTF and HRRN both depend on honest burst declarations, which is unrealistic for interactive processes whose next burst is unpredictable.

### Starvation
SRTF has no guard: a steady stream of short jobs can postpone a long job indefinitely. HRRN solves this elegantly, since wait grows every tick while burst stays fixed, so the response ratio increases without bound and the long job eventually overtakes new arrivals. MLFQ mitigates the problem through the interactive boost but has no explicit aging across queues, so a CPU-bound process at level 2 can still wait a long time under heavy interactive load.

### Verdict
- **SRTF** for batch systems with reliable burst estimates and turnaround as the goal.
- **HRRN** for batch systems where fairness matters and preemption overhead should be avoided.
- **MLFQ** for general-purpose and interactive systems, which is why real operating systems use variants of it.

---

## Directory Structure

```
Asg4_Submission/
├── README.md          # this file: consolidated report and comparison
├── q1/                # SRTF
│   ├── README.md, srtftest.c, proc.c, proc.h, trap.c,
│   │   sysproc.c, syscall.c, syscall.h, defs.h, user.h, usys.S, Makefile
│   ├── diffs/         # unified diffs vs pristine xv6-public
│   └── q1_srtf.png
├── q2/                # HRRN
│   ├── README.md, hrrntest.c, same kernel file set
│   ├── diffs/
│   └── q2_hrrn.png
└── q3/                # MLFQ
    ├── README.md, mlfqtest.c, same kernel file set
    ├── diffs/
    └── q3_mlfq.png
```

---

## Build and Run

Each question is an independent xv6 tree. Start from pristine xv6, overlay the question files, then build.

```bash
cp -r xv6-clean asg4_test && cd asg4_test
cp ../Asg4_Submission/q1/* .        # or q2/ or q3/
sed -i "s/-Werror//g" Makefile
make clean && make qemu-nox
```

At the xv6 shell prompt:

```
$ srtftest      # Q1 SRTF
$ hrrntest      # Q2 HRRN
$ mlfqtest      # Q3 MLFQ
```

Exit QEMU with Ctrl+A then X.

### Two required build notes
1. **Remove `-Werror`.** Modern GCC flags the intentional recursion in the stock `sh.c` as an error, which aborts the build. Removing the flag leaves the generated code unchanged.
2. **Use one CPU.** Q2 and Q3 set `CPUS := 1` in the Makefile so that scheduling order is deterministic and the printed completion order is meaningful.

### Reproducing from diffs
```bash
cp -r xv6-clean rebuild && cd rebuild
for d in ../Asg4_Submission/q1/diffs/*.diff; do patch -p0 < "$d"; done
sed -i "s/-Werror//g" Makefile
make clean && make qemu-nox
```
