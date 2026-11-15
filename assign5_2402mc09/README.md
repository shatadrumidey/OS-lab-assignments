# xv6 Process Synchronization Lab

This repository contains the solutions for **Operating Systems Lab Assignment 6 (MA3105)**.

The assignment focuses on **process synchronization** using the xv6 operating system. We implemented three classic synchronization problems:

1. Peterson's Algorithm
2. Producer-Consumer Problem
3. Readers-Writers Problem

Since xv6 processes normally have separate memory, we also added **shared memory** and **kernel-level semaphores** to make these problems possible.
 
**Name:** Shatadrumi Dey
**Roll No.:** 2402MC09

---

## Repository Structure

Each question has its own folder.

- `q1-peterson/` - Peterson's Algorithm for mutual exclusion
- `q2-prodcons/` - Producer-Consumer using a bounded buffer
- `q3-readwrite/` - Fair Readers-Writers solution

Each folder contains:

- The modified xv6 source files
- A `README.md` explaining the solution
- A `diffs/` folder showing the changes made to xv6
- Screenshots of the program output

---

# Kernel Changes

Two important features were added to xv6 and used in the three questions.

## 1. Shared Memory - `shm_get()`

Normally, when we use `fork()`, the parent and child processes have separate memory.

For example, if both processes have:

```c
int counter = 0;
```

Changing `counter` in one process does not change it in the other process.

To solve this, we added a new system call:

```c
shm_get()
```

This system call:

- Allocates one physical memory page using `kalloc()`
- Maps it at a fixed virtual address: `SHM_VA = 0x60000000`
- Allows different processes to access the same physical memory

The parent and child can therefore use the same variables.

For example, in Peterson's Algorithm, `flag[]`, `turn`, and `counter` are stored in this shared page.

We also modified `vm.c` so that xv6 does not accidentally free this shared page when a process exits.

---

## 2. Kernel Semaphores

xv6 already provides locks such as spinlocks, but it does not provide normal counting semaphores.

So we added our own kernel semaphore.

A semaphore contains:

```c
struct ksem {
    struct spinlock lock;
    int count;
};
```

The main operations are:

```c
sem_init()
sem_wait()
sem_signal()
```

### `sem_init()`

Sets the initial value of the semaphore.

### `sem_wait()`

If the semaphore has a positive count, the process continues and the count is decreased.

If the count is zero, the process goes to sleep until another process signals the semaphore.

### `sem_signal()`

Increases the semaphore count and wakes up a process waiting on the semaphore.

The implementation uses xv6's existing:

```c
sleep()
wakeup()
```

mechanisms.

---

# Question 1 - Peterson's Algorithm

Peterson's Algorithm is used to provide **mutual exclusion between two processes**.

The two processes share:

```c
flag[2]
turn
counter
```

The problem is that these variables cannot normally be shared between xv6 processes.

We solved this using `shm_get()`.

Both the parent and child access the same physical memory page, so they can correctly update `flag[]` and `turn`.

The critical section performs a:

```text
read -> delay -> write
```

operation on the shared counter.

Both processes perform this operation 10 times.

The expected final value is:

```text
20
```

Getting exactly 20 shows that both processes were prevented from entering the critical section at the same time.

Importantly, the mutual exclusion is provided by **Peterson's Algorithm itself**, rather than by an xv6 lock.

---

# Question 2 - Producer-Consumer

The Producer-Consumer problem uses a **bounded buffer**.

In this implementation:

- Buffer size = 5
- Items = 20
- Producers = 1
- Consumers = 1

We use three semaphores:

```text
empty = 5
full  = 0
mutex = 1
```

### `empty`

Keeps track of how many empty spaces are available in the buffer.

Initially there are 5 empty spaces.

### `full`

Keeps track of how many items are currently in the buffer.

Initially there are no items.

### `mutex`

Makes sure that only one process accesses the buffer at a time.

The Producer adds items to the buffer.

The Consumer removes items from the buffer.

The Producer is intentionally made faster than the Consumer. Because the buffer can only hold 5 items, the Producer eventually has to wait when the buffer becomes full.

When the Consumer removes an item, it signals `empty`, allowing the Producer to continue.

The output demonstrates this blocking and waking behavior.

All 20 items are produced and consumed in the correct order, with no missing or duplicate items.

---

# Question 3 - Readers-Writers

The Readers-Writers problem involves multiple readers and writers accessing the same shared data.

This implementation uses:

- 3 Readers
- 2 Writers

The important rules are:

- Multiple readers can read at the same time.
- Only one writer can write at a time.
- A writer cannot access the data while readers are reading.
- Writers should not be starved by continuously arriving readers.

## Fair Readers-Writers Solution

A simple reader-priority solution can cause **writer starvation**.

For example:

```text
Reader
Reader
Reader
Reader
Reader
...
```

If new readers keep arriving, a waiting writer may never get a chance to run.

To avoid this, we use an additional semaphore called:

```text
readTry
```

This acts like a gate for readers.

When a writer is waiting, new readers cannot simply enter. They have to wait behind the writer.

This gives writers a fair chance to access the shared data.

The output demonstrates that:

- Multiple readers can access the data together.
- Writers access the data exclusively.
- Writers are able to run even when readers keep arriving.
- There are no overlapping writer executions.

---

# How to Build and Run

## Requirements

You need:

- Linux or WSL
- `gcc`
- `make`
- `qemu-system-i386`

## Run Question 1

```bash
cd q1-peterson
make clean
make qemu-nox
```

## Run Question 2

```bash
cd q2-prodcons
make clean
make qemu-nox
```

## Run Question 3

```bash
cd q3-readwrite
make clean
make qemu-nox
```

Once xv6 starts, run the corresponding user program from the xv6 shell.

---

# Summary

This assignment adds two synchronization features to xv6:

### Shared Memory

Allows different processes to access the same physical memory.

### Kernel Semaphores

Allow processes to coordinate their execution by waiting and waking up when necessary.

These features were then used to implement:

| Question | Problem | Main Concept |
|----------|---------|--------------|
| Q1 | Peterson's Algorithm | Mutual Exclusion |
| Q2 | Producer-Consumer | Counting Semaphores |
| Q3 | Readers-Writers | Fair Synchronization |

Together, these implementations demonstrate how processes can safely share data and coordinate their execution inside xv6.
