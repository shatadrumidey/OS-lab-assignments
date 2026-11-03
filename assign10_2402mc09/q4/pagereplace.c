#include "types.h"
#include "stat.h"
#include "user.h"

#define MAXFRAMES 10
#define MAXREF    20

static int ref[MAXREF] = {7,0,1,2,0,3,0,4,2,3,0,3,2,1,2,0,1,7,0,1};
static int nref = MAXREF;

static void
print_frames(int *frames, int nf)
{
  int i;
  for(i = 0; i < nf; i++) printf(1, "%d ", frames[i]);
  for(; i < MAXFRAMES; i++) printf(1, "- ");
}

static int
fifo(int nf)
{
  int frames[MAXFRAMES], nframes = 0, ptr = 0;
  int i, j, faults = 0, hits = 0;
  printf(1, "\n=== FIFO with %d frames ===\n", nf);
  for(i = 0; i < nref; i++){
    int r = ref[i], hit = 0;
    for(j = 0; j < nframes; j++)
      if(frames[j] == r){ hit = 1; break; }
    if(hit){
      hits++;
      printf(1, "ref %2d: %d -> HIT   frames: ", i, r);
    } else {
      faults++;
      if(nframes < nf){ frames[nframes++] = r; }
      else            { frames[ptr] = r; ptr = (ptr+1) % nf; }
      printf(1, "ref %2d: %d -> FAULT frames: ", i, r);
    }
    print_frames(frames, nframes);
    printf(1, "\n");
  }
  printf(1, "FIFO(%d): faults=%d hits=%d fault_ratio=%.2f hit_ratio=%.2f\n",
         nf, faults, hits, (float)faults/(float)nref, (float)hits/(float)nref);
  return faults;
}

static int
lru(int nf)
{
  int frames[MAXFRAMES], last_use[MAXFRAMES], nframes = 0;
  int i, j, faults = 0, hits = 0;
  printf(1, "\n=== LRU with %d frames ===\n", nf);
  for(i = 0; i < nref; i++){
    int r = ref[i], hit = 0, lru_idx = 0;
    for(j = 0; j < nframes; j++){
      if(frames[j] == r){ hit = 1; last_use[j] = i; break; }
      if(last_use[j] < last_use[lru_idx]) lru_idx = j;
    }
    if(hit){
      hits++;
      printf(1, "ref %2d: %d -> HIT   frames: ", i, r);
    } else {
      faults++;
      if(nframes < nf){
        frames[nframes] = r; last_use[nframes] = i; nframes++;
      } else {
        frames[lru_idx] = r; last_use[lru_idx] = i;
      }
      printf(1, "ref %2d: %d -> FAULT frames: ", i, r);
    }
    print_frames(frames, nframes);
    printf(1, "\n");
  }
  printf(1, "LRU(%d): faults=%d hits=%d fault_ratio=%.2f hit_ratio=%.2f\n",
         nf, faults, hits, (float)faults/(float)nref, (float)hits/(float)nref);
  return faults;
}

static int
optimal(int nf)
{
  int frames[MAXFRAMES], nframes = 0;
  int i, j, k, faults = 0, hits = 0;
  printf(1, "\n=== OPTIMAL with %d frames ===\n", nf);
  for(i = 0; i < nref; i++){
    int r = ref[i], hit = 0;
    for(j = 0; j < nframes; j++)
      if(frames[j] == r){ hit = 1; break; }
    if(hit){
      hits++;
      printf(1, "ref %2d: %d -> HIT   frames: ", i, r);
    } else {
      faults++;
      if(nframes < nf){
        frames[nframes++] = r;
      } else {
        // Find page whose next use is farthest in future
        int farthest = -1, replace_idx = 0;
        for(j = 0; j < nf; j++){
          int next_use = nref; // default: never used again
          for(k = i+1; k < nref; k++){
            if(frames[j] == ref[k]){ next_use = k; break; }
          }
          if(next_use > farthest){
            farthest = next_use;
            replace_idx = j;
          }
        }
        frames[replace_idx] = r;
      }
      printf(1, "ref %2d: %d -> FAULT frames: ", i, r);
    }
    print_frames(frames, nframes);
    printf(1, "\n");
  }
  printf(1, "OPTIMAL(%d): faults=%d hits=%d fault_ratio=%.2f hit_ratio=%.2f\n",
         nf, faults, hits, (float)faults/(float)nref, (float)hits/(float)nref);
  return faults;
}

int
main(int argc, char *argv[])
{
  int nf = 3;
  if(argc > 1) nf = atoi(argv[1]);
  if(nf < 1 || nf > MAXFRAMES){ printf(1, "frames must be 1..%d\n", MAXFRAMES); exit(); }

  printf(1, "Reference string (%d refs): ", nref);
  for(int i = 0; i < nref; i++) printf(1, "%d ", ref[i]);
  printf(1, "\n");

  fifo(nf);
  lru(nf);
  optimal(nf);

  printf(1, "\n=== Comparison Table (3, 4, 5 frames) ===\n");
  printf(1, "frames | FIFO faults | LRU faults | OPT faults | FIFO hit% | LRU hit% | OPT hit%\n");
  for(nf = 3; nf <= 5; nf++){
    int ff = fifo(nf);
    int lf = lru(nf);
    int of = optimal(nf);
    printf(1, "%6d | %11d | %10d | %10d | %9.2f | %8.2f | %8.2f\n",
           nf, ff, lf, of,
           (float)(nref-ff)/(float)nref,
           (float)(nref-lf)/(float)nref,
           (float)(nref-of)/(float)nref);
  }
  printf(1, "\nBest algorithm for this string: OPTIMAL (lowest faults, highest hit ratio)\n");
  printf(1, "pagereplace done\n");
  exit();
}
