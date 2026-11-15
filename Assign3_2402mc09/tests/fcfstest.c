#include "types.h"
#include "stat.h"
#include "user.h"

int main(int argc, char *argv[]) {
  int i, pid;
  
  printf(1, "FCFS Test Program Started\n");

  for (i = 0; i < 3; i++) {
    pid = fork();
    if (pid < 0) {
      printf(1, "Fork failed\n");
      exit();
    } else if (pid == 0) {
      // Child process: CPU-bound intensive loop
      int j, k;
      volatile int val = 0;
      for (j = 0; j < 40000; j++) {
        for (k = 0; k < 10000; k++) {
          val += j * k; // Heavy computation loop
        }
      }
      printf(1, "Process Finished: PID %d\n", getpid());
      exit();
    } else {
      // Parent process: small delay loop to stagger creation times cleanly
      int d, m;
      for (d = 0; d < 10000; d++) {
        for (m = 0; m < 1000; m++) {
          // empty delay
        }
      }
    }
  }

  // Wait for all three children to finish
  for (i = 0; i < 3; i++) {
    wait();
  }

  printf(1, "All processes completed under FCFS scheduler.\n");
  exit();
}
