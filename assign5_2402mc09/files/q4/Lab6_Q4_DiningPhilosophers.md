# Lab 6 Question 4: Dining Philosophers — Avoiding Deadlock

## Strategy used: Fork ordering (resource ordering)

Each fork is a binary semaphore (the custom `ksem` built in Q2, based on
xv6's `sleep()`/`wakeup()`). Philosopher `i` needs fork `i` and fork
`(i+1)%5`.

The rule: every philosopher must pick up the **lower-numbered fork first**,
then the higher-numbered one. Because of this, philosopher 4 picks up fork
0 before fork 4 — breaking the usual pattern where everyone just grabs the
fork on their left first.

## Why this stops deadlock from happening

Deadlock needs a "circular wait" — a loop where everyone is stuck waiting
on the next person, all the way around back to themselves.

With this fork-ordering rule, whenever a philosopher is stuck waiting, they
are always waiting for a fork with a **higher number** than the one they
already have. So going around the chain of "who's waiting for whom," the
fork numbers only ever go up.

For a circular wait to happen, the numbers would have to go up and
eventually loop back to where they started — but that's impossible since
they only increase. So a circular wait can never form, which means
deadlock can never happen either.

## Other options I considered

- **Room semaphore:** only allow 4 philosophers at the table at once. This
  also prevents deadlock, but it needs an extra semaphore and reduces how
  many philosophers can act at the same time.
- **Odd/even pickup order:** basically the same idea as fork ordering,
  just described differently.

I went with fork ordering since it needs the least code, no extra
semaphores, and still allows full concurrency.

## Fairness — no one gets stuck waiting forever

Eating and thinking times are both limited (`sleep(10)` and `sleep(5)`),
and a philosopher always puts both forks down after eating. So no one gets
skipped forever.

In the output, all 5 philosophers complete all 5 rounds and the program
ends cleanly. If there were a deadlock, the program would just hang
forever instead.

## How to build and run it
```
make clean && make qemu-nox
$ dining
```
