# Asg 4 Q3: Multilevel Feedback Queue (MLFQ) Scheduler

**Student:** Shaan Shan | **Roll No:** 2402MC08 | **Course:** MA3105

## Overview
Simplified MLFQ with 3 priority queues. CPU-heavy processes are penalised by demotion, while interactive and I/O-bound processes keep high priority.

## Design
- **proc.h:** added `int queue_level` (0 = highest, 2 = lowest) and `int ticks_used` to `struct proc`.
- **proc.c (allocproc):** every new process starts at level 0 with ticks_used 0.
- **proc.c (scheduler):** strict priority across queues. It scans level 0 first, then 1, then 2, and only considers the next level when the current one has no RUNNABLE process. Inside a level a rotating cursor gives Round-Robin ordering.
- **trap.c (timer interrupt):** increments `ticks_used` of the running process and compares it with the quantum of its queue:
```
level 0 -> 1 tick
level 1 -> 2 ticks
level 2 -> 4 ticks
```
  When the full quantum is consumed the process is demoted (`queue_level++`, capped at 2), `ticks_used` is reset, and `yield()` is called.
- **proc.c (sleep):** a voluntary block resets `ticks_used` and restores `queue_level` to 0. This is the interactive boost: a process that keeps blocking for I/O never accumulates a full quantum, so it stays at the highest priority.
- **proc.c (get_queue_level) + sys_getqlevel:** reporting syscall so the test can print the live queue level of any pid.
- **Makefile:** `CPUS := 1` for deterministic, readable output.

## Test Program (mlfqtest.c)
- Forks 4 children: CPU1 and CPU2 run long busy loops with no sleep; IO1 and IO2 call `sleep(2)` forty times.
- The parent samples `getqlevel()` for all four children every 10 ticks and prints a round-by-round table.
- Each child also prints its own final queue level when it finishes.

## Observed Results (q3_mlfq.png)
- CPU1 and CPU2 drop from level 0 to level 1 and then to level 2 within the first few rounds, and stay at 2.
- IO1 and IO2 remain at level 0 for their whole lifetime because each `sleep()` resets their slice.
- A level of -1 in the table means the process has already exited and its slot was reclaimed.
- Confirms the expected outcome: CPU-bound work sinks to the lowest queue, frequently yielding work stays at the top.

## Build and Run
```bash
cp -r xv6-clean asg4_test && cd asg4_test
cp ../Asg4_Submission/q3/* .
sed -i "s/-Werror//g" Makefile
make clean && make qemu-nox
```
At the xv6 prompt: `mlfqtest`. Exit QEMU with Ctrl+A then X.
