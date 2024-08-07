#include "riscv.h"
#define NPAGE (PHYSTOP/PGSIZE)
// Mutual exclusion lock.
struct spinlock {
  uint locked;       // Is the lock held?

  // For debugging:
  char *name;        // Name of lock.
  struct cpu *cpu;   // The cpu holding the lock.
};

struct 
{
  struct spinlock lock;
  int counting[NPAGE];
}lock_counting;
