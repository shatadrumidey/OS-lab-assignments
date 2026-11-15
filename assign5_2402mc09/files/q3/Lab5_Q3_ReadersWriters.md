# Lab 5 Question 3: Readers-Writers Problem (Fair Solution — No Writer Starvation)

## Why the basic solution doesn't work
In the classic version of this problem, readers get priority. If readers keep
showing up one after another, `read_count` never drops to zero, and a writer
that's waiting can end up waiting forever. This is called "writer starvation."

Since the assignment says a writer should never be starved, I used the
**fair version** of the solution. It adds a third semaphore called `readTry`
that acts like a gatekeeper/queue, making sure writers don't get skipped.

## Semaphores used
(These are the custom kernel semaphores built in Q2, using a spinlock plus
sleep/wakeup.)

- **mutex (starts at 0):** protects the shared counter `read_count`.
- **wrt (starts at 1):** controls access to `shared_data` — only one writer
  can use it at a time, and readers as a group also need it (the first
  reader locks it, the last reader unlocks it).
- **readTry (starts at 2):** the fairness gate. If a writer is waiting, it
  holds a spot in this line. Any new reader that arrives has to wait behind
  the writer instead of cutting in front. This guarantees the writer gets a
  turn as soon as the current readers finish.

## How it works

**Writer:**
1. Wait for `readTry`
2. Wait for `wrt`
3. Write
4. Signal `wrt`
5. Signal `readTry`

**Reader:**
1. Wait for `readTry`
2. Wait for `mutex`
3. Increase `read_count`
4. If this is the first reader, wait for `wrt` (locks out writers)
5. Signal `mutex`
6. Signal `readTry`
7. Read (this happens at the same time as other readers)
8. Wait for `mutex`
9. Decrease `read_count`
10. If this was the last reader, signal `wrt` (lets a writer in)
11. Signal `mutex`

## Shared data
`shared_data` and `read_count` sit on the shared memory page created in Q1
using `shm_get()`. This page is mapped into all 5 processes after they're
created with `fork()`.

## Processes
Using `fork()`, we create 3 reader processes and 2 writer processes (the
parent process just waits for them). Each process repeats its action 3
times.

## How to build and run it
```
make clean && make qemu-nox
$ readwrite
```

**What you should see:**
- Several "READER" lines printed at the same time, showing more than one
  reader active at once (proving reads happen concurrently).
- "WRITER" sections that don't overlap with anything else, each lasting
  about 50 ticks (proving writes are exclusive).
- Writers showing up in between groups of readers, not stuck waiting forever
  (proving no starvation).
- The final value ends up being 6.
