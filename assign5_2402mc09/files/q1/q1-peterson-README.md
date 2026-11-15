# Lab 5 Question 1: Peterson's Algorithm using Shared Memory

## Design Overview

Peterson's Algorithm is used to make sure that two processes do not enter the same critical section at the same time.

In xv6, processes normally have separate memory after `fork()`. Because of this, we cannot directly use normal variables for Peterson's Algorithm.

To solve this, we added a simple shared memory system.

### 1. Shared Memory

We added a new system call:

```c
int shm_get(void);
```

It creates one shared memory page using `kalloc()` and maps it at a fixed virtual address:

```c
SHM_VA = 0x60000000
```

The parent and child call `shm_get()` after `fork()`. Both processes then get access to the same physical memory page.

This allows them to share:

- `flag[2]`
- `turn`
- `counter`

These variables are needed by Peterson's Algorithm.

### 2. Sharing the Same Physical Page

The important part is that both processes' page tables point to the **same physical memory page**.

Therefore, when one process changes a shared variable, the other process can see the change immediately.

Without this, each process would have its own copy of `flag`, `turn`, and `counter`, and Peterson's Algorithm would not work correctly.

### 3. Protecting the Shared Page

We also made a change in `vm.c`.

Normally, xv6 frees a process's memory when the process exits. Since the shared page is still being used by another process, freeing it could cause a **use-after-free** problem.

So `deallocuvm()` was modified to skip the shared page at `SHM_VA`.

### 4. Peterson's Algorithm

The program `peterson.c` uses the normal Peterson's Algorithm entry and exit protocol.

It uses only:

- `flag[]`
- `turn`
- shared memory

No xv6 locks or semaphores are used for the critical section.

The critical section performs a:

```text
read -> delay -> write
```

operation on the shared counter.

This delay makes a race condition easier to observe if mutual exclusion is not working.

---

## Files Modified

The following xv6 files were modified:

- `proc.h` and `proc.c` - Added the shared memory page and `shm_get()` implementation.
- `vm.c` - Added the shared memory mapping code and protected the shared page from being freed.
- `memlayout.h` - Defined `SHM_VA = 0x60000000`.
- `syscall.c`, `syscall.h`, `sysproc.c`, `defs.h`, `usys.S`, `user.h` - Added and connected the `shm_get` system call.
- `Makefile` - Added `_peterson` to the list of user programs.

---

## Build and Run

### 1. Extract the submission folder

Unzip the submission folder and open a terminal inside it.

### 2. Build xv6

Run:

```bash
make clean
make qemu-nox
```

### 3. Run the program

When the xv6 shell appears, run:

```text
$ peterson
```

### 4. Expected Output

The output will show Process 0 and Process 1 taking turns and updating the shared counter.

Both processes perform the critical section multiple times.

If Peterson's Algorithm is working correctly, the final counter should be:

```text
Final counter = 20
```

The expected value is also `20`.

This shows that both processes were able to safely update the shared counter without entering the critical section at the same time.
