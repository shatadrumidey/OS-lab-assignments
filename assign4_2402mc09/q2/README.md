# Asg 4 Q2: Highest Response Ratio Next (HRRN) Scheduler

**Student:** Shaan Shan | **Roll No:** 2402MC08 | **Course:** MA3105

## Overview
Non-preemptive HRRN scheduler. Balances the short-job preference of SJF with starvation prevention for long jobs.

## Design
- **proc.h:** added `int ctime` and `int burst_time` to `struct proc`.
- **proc.c (allocproc):** `p->ctime = ticks` and `p->burst_time = 0`.
- **proc.c (set_burst):** kernel helper; validates bt >= 1 and stores the declared burst for the given pid.
- **sysproc.c (sys_set_burst):** syscall wrapper reading (pid, bt) with argint.
- **proc.c (scheduler):** for every RUNNABLE process with a declared burst it computes, using pure integer arithmetic:
```
wait = ticks - ctime          (clamped to 0 if negative)
rr   = ((wait + burst) * 100) / burst
```
  The process with the highest rr is selected. If no process has declared a burst (for example init or sh), the scheduler falls back to the first RUNNABLE process so the system stays usable.
- **trap.c:** the timer-interrupt `yield()` is commented out. This makes the scheduler genuinely non-preemptive: the selected process runs until it exits or blocks.
- **Makefile:** `CPUS := 1` so completion order is deterministic and easy to verify.

## Why starvation cannot happen
For a waiting process, `wait` grows every tick while `burst` stays fixed, so its response ratio increases without bound. A long job therefore overtakes newly queued short jobs after waiting long enough, guaranteeing it eventually runs.

## Test Program (hrrntest.c)
- Forks 5 children. P1 is a very long job (burst 300) created FIRST; P2..P5 are short jobs (15, 10, 20, 25).
- The parent calls `set_burst(child_pid, bt)` right after each fork, so all bursts are known before any child is scheduled.
- Each child runs a CPU-bound loop proportional to its declared burst and prints start and done with its PID.

## Observed Results (q2_hrrn.png)
- Completion order: P3 (10), P2 (15), P4 (20), P5 (25), P1 (300).
- Short jobs are served early, exactly as the response ratio predicts.
- P1, despite being the longest job, still completes: its rising wait time lifted its ratio, so no starvation occurred.

## Build and Run
```bash
cp -r xv6-clean asg4_test && cd asg4_test
cp ../Asg4_Submission/q2/* .
sed -i "s/-Werror//g" Makefile
make clean && make qemu-nox
```
At the xv6 prompt: `hrrntest`. Exit QEMU with Ctrl+A then X.
