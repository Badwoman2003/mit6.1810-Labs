#include "types.h"
#include "riscv.h"
#include "defs.h"
#include "param.h"
#include "memlayout.h"
#include "spinlock.h"
#include "proc.h"

uint64
sys_exit(void)
{
  int n;
  argint(0, &n);
  exit(n);
  return 0; // not reached
}

uint64
sys_getpid(void)
{
  return myproc()->pid;
}

uint64
sys_fork(void)
{
  return fork();
}

uint64
sys_wait(void)
{
  uint64 p;
  argaddr(0, &p);
  return wait(p);
}

uint64
sys_sbrk(void)
{
  uint64 addr;
  int n;

  argint(0, &n);
  addr = myproc()->sz;
  if (growproc(n) < 0)
    return -1;
  return addr;
}

uint64
sys_sleep(void)
{
  int n;
  uint ticks0;

  argint(0, &n);
  if (n < 0)
    n = 0;
  acquire(&tickslock);
  ticks0 = ticks;
  while (ticks - ticks0 < n)
  {
    if (killed(myproc()))
    {
      release(&tickslock);
      return -1;
    }
    sleep(&ticks, &tickslock);
  }
  release(&tickslock);
  return 0;
}

uint64
sys_kill(void)
{
  int pid;

  argint(0, &pid);
  return kill(pid);
}

// return how many clock tick interrupts have occurred
// since start.
uint64
sys_uptime(void)
{
  uint xticks;

  acquire(&tickslock);
  xticks = ticks;
  release(&tickslock);
  return xticks;
}

// after n ticks return function address
uint64
sys_sigalarm(void)
{
  struct proc *p = myproc();
  argint(0, &p->ticks);              // first arg:ticks
  argaddr(1, (uint64 *)&p->handler); // second arg:adress
  p->ticks_count = 0;                // init:make count = 0
  if ((!p->ticks) && (!p->handler))
  {
    p->ticks = -1; // unusual situation:sigalarm(0,0)
  }
  return 0;
}

// return from sigalarm
uint64
sys_sigreturn(void)
{
  struct proc *p = myproc();
  p->trapframe->epc = p->epc; //the expcetion address before sigalarm
  p->trapframe->ra = p->ra;
  p->trapframe->sp = p->sp;
  p->trapframe->s0 = p->s0;
  p->trapframe->a1 = p->a1;
  p->proc_flag = 0;
  return p->a0; // When a signal is handled, the system call return value is stored in p->trapframe->a0.
              // p->a0 temporarily holds this value so that it can be returned to the user process
              // after the signal handler finishes and the original state is restored.
}