# Lab 5 Question 2: Producer-Consumer (Bounded Buffer) in xv6

## Semaphore Implementation

For this question, we implemented our own counting semaphores inside the xv6 kernel.

The semaphore is defined as:

```c
struct ksem {
    struct spinlock lock;
    int count;
};
```

The implementation uses xv6's existing `spinlock`, `sleep()`, and `wakeup()` functions.

### `sem_wait()`

When a process calls `sem_wait()`:

1. It acquires the semaphore lock.
2. If the count is `0`, the process goes to sleep.
3. When it is woken up, it checks the count again.
4. If the count is available, it decreases the count and continues.
5. Finally, it releases the lock.

The `sleep()` function releases the lock while the process is sleeping and gets the lock again when the process wakes up. This prevents lost wakeups.

### `sem_signal()`

When a process calls `sem_signal()`:

1. It acquires the semaphore lock.
2. Increases the count.
3. Wakes up a waiting process.
4. Releases the lock.

The semaphore functions were made available to user programs through three new system calls:

```c
sem_init(which, count)
sem_wait(which)
sem_signal(which)
```

The required syscall files were also updated, including `syscall.h`, `syscall.c`, `sysproc.c`, `defs.h`, `user.h`, and `usys.S`.

For the Producer-Consumer problem, three semaphores are used:

```text
empty = 5
full  = 0
mutex = 1
```

Here, the buffer size is `5`.

---

## Shared Memory

The shared memory mechanism from Question 1 is reused here.

The parent and child both call `shm_get()` after `fork()`. This maps the same physical memory page at:

```text
SHM_VA = 0x60000000
```

The circular buffer and its indexes are stored in this shared page:

```text
buffer[5]
in
out
```

This means both the Producer and Consumer can directly access the same buffer.

We use shared memory for the data itself and semaphores for synchronization.

This follows the standard bounded-buffer design instead of using pipes.

---

## Producer and Consumer Behavior

There is one Producer and one Consumer.

The Producer is intentionally made faster:

```text
Producer: sleep(2)
Consumer: sleep(20)
```

Because the Producer is faster, it quickly fills the buffer.

Once all 5 slots are full, the Producer calls:

```c
sem_wait(empty)
```

and has to wait until the Consumer removes an item.

This can be seen in the output as a pause after the Producer fills the buffer.

When the Consumer removes an item, it signals `empty`, allowing the Producer to continue.

The Consumer also waits when the buffer is empty:

```c
sem_wait(full)
```

The `mutex` semaphore makes sure that only one process modifies the buffer indexes and buffer contents at a time.

The overall flow is:

```text
Producer
    |
    v
wait(empty)
    |
wait(mutex)
    |
insert item
    |
signal(mutex)
    |
signal(full)
```

and:

```text
Consumer
    |
    v
wait(full)
    |
wait(mutex)
    |
remove item
    |
signal(mutex)
    |
signal(empty)
```

The Producer and Consumer continue this process until all 20 items have been processed.

---

## Build and Run

Run:

```bash
make clean
make qemu-nox
```

When the xv6 shell appears, run:

```text
$ prodcons
```

---

## Expected Output

The output should show:

```text
Producer: inserted 1
Producer: inserted 2
...
Producer: inserted 20

Consumer: removed 1
Consumer: removed 2
...
Consumer: removed 20
```

Because the Producer is faster than the Consumer, there should be visible pauses when the buffer becomes full.

The Consumer may also block when the buffer becomes empty.

The final result should show that all 20 items were:

- Produced
- Consumed
- Kept in the correct order
- Not lost
- Not duplicated

This confirms that the bounded buffer and semaphore synchronization are working correctly.
