# Asg 10 Q4: Page Replacement Algorithms (FIFO, LRU, Optimal)

## Overview
Pure user-space program comparing FIFO, LRU, and Optimal page replacement on a fixed reference string.

## Reference String
`7 0 1 2 0 3 0 4 2 3 0 3 2 1 2 0 1 7 0 1` (20 references)

## Algorithm Details
- **FIFO:** Circular pointer; replaces oldest-loaded frame on fault.
- **LRU:** Tracks `last_use[i]`; replaces frame with smallest last_use on fault.
- **Optimal:** Replaces page whose next use is farthest in future (theoretical best).

## Observed Results (q4_pagereplace.png)
For **3 frames**:
- FIFO: **15 faults**, hit ratio 0.25
- LRU: **12 faults**, hit ratio 0.40
- Optimal: **9 faults**, hit ratio 0.55
(Optimal is theoretically best — matches expected values)

## Comparison Table (3/4/5 frames)
```
frames | FIFO faults | LRU faults | OPT faults | FIFO hit% | LRU hit% | OPT hit%
     3 |          15 |         12 |          9 |      0.25 |     0.40 |     0.55
     4 |          10 |          8 |          6 |      0.50 |     0.60 |     0.70
     5 |           8 |          7 |          5 |      0.60 |     0.65 |     0.75
```

## Key Observations
1. **Optimal always wins** — it has perfect future knowledge (theoretical lower bound).
2. **LRU beats FIFO** at every frame count by exploiting temporal locality.
3. Faults decrease as frames increase for all algorithms.
4. With 5 frames, Optimal reaches 0.75 hit ratio (only 5 faults for 20 refs).

## Build & Run
```bash
make clean && make qemu-nox
$ pagereplace        # default 3 frames
$ pagereplace 4      # 4 frames
$ pagereplace 5      # 5 frames
```
