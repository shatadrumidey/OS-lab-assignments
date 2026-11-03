# Asg 9 Q3: Page Replacement (FIFO and LRU)

## Overview
Pure user-space program comparing FIFO and LRU on a fixed reference string. **No kernel changes.**

## Reference String
`7 0 1 2 0 3 0 4 2 3 0 3 2 1 2 0 1 7 0 1` (20 references)

## Algorithm Details
- **FIFO:** Circular pointer; replaces oldest-loaded frame on fault.
- **LRU:** Tracks `last_use[i]`; replaces frame with smallest last_use on fault.

## Observed Results (q3_pagerepl.png)
For **3 frames**:
- FIFO: **15 faults**, 5 hits, ratio 0.25
- LRU: **12 faults**, 8 hits, ratio 0.40
(Matches expected: 15 FIFO, 12 LRU ✓)

## Summary Table (3/4/5 frames)
```
frames | FIFO faults | LRU faults | FIFO ratio | LRU ratio
     3 |          15 |         12 |       0.25 |      0.40
     4 |          10 |          8 |       0.50 |      0.60
     5 |           8 |          7 |       0.60 |      0.65
```

## Key Observations
1. LRU outperforms FIFO at every frame count (exploits locality).
2. Faults decrease as frames increase for both algorithms.
3. With 5 frames, both approach optimal (only 7 distinct pages in string).

## Build & Run
```bash
make clean && make qemu-nox
$ pagerepl        # default 3 frames
$ pagerepl 4      # 4 frames
$ pagerepl 5      # 5 frames
```
