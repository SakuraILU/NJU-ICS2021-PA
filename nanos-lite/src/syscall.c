#include <common.h>
#include "syscall.h"
#include <fs.h>
#include <proc.h>

// static intptr_t heap_brk;
// static int brk(intptr_t addr);

typedef struct timeval
{
  int32_t tv_sec;  /* seconds */
  int32_t tv_usec; /* microseconds */
} Timeval;
typedef struct timezone
{
  int tz_minuteswest; /* minutes west of Greenwich */
  int tz_dsttime;     /* type of DST correction */
} Timezone;
static int gettimeofday(Timeval *tv, Timezone *tz);

void naive_uload(PCB *pcb, const char *filename);
int execve(const char *pathname, char *const argv[], char *const envp[]);
void exit(int status);
int mm_brk(uintptr_t brk);

void do_syscall(Context *c)
{
  uintptr_t a[4];
  a[0] = c->GPR1;

  switch (a[0])
  {
  case SYS_exit:
    exit((int)c->GPR2);
    break;
  case SYS_yield:
    yield();
    c->GPRx = 0;
    break;
  case SYS_open:
    c->GPRx = fs_open((const char *)c->GPR2, (int)c->GPR3, (int)c->GPR4);
    break;
  case SYS_write:
    c->GPRx = fs_write((int)c->GPR2, (const void *)c->GPR3, (size_t)c->GPR4);
    break;
  case SYS_read:
    c->GPRx = fs_read((int)c->GPR2, (void *)c->GPR3, (size_t)c->GPR4);
    break;
  case SYS_lseek:
    c->GPRx = fs_lseek((int)c->GPR2, (size_t)c->GPR3, (int)c->GPR4);
    break;
  case SYS_close:
    c->GPRx = fs_close((int)c->GPR2);
    break;
  case SYS_execve:
    c->GPRx = execve((const char *)c->GPR2, (char **const)c->GPR3, (char **const)c->GPR4);
    break;
  case SYS_brk:
    c->GPRx = (int)mm_brk((uintptr_t)c->GPR2);
    break;
  case SYS_gettimeofday:
    c->GPRx = (int)gettimeofday((Timeval *)c->GPR2, (Timezone *)c->GPR3);
    break;
  default:
    panic("Unhandled syscall ID = %d", a[0]);
  }
}

// static int brk(intptr_t addr)
// {
//   // printf("the increment is %d\n", (int)increment);
//   heap_brk = (intptr_t)addr;
//   // printf("brk is %p\n",heap_brk);
//   return 0;
// }

static AM_TIMER_UPTIME_T uptime;
static int gettimeofday(Timeval *tv, Timezone *tz)
{
  ioe_read(AM_TIMER_UPTIME, &uptime);
  // important: must convert to int32_t as tv_usec/sec is int32_t
  //            uptime.us is uint64_t, cause overflow!!!!!!!!!!!
  //            make the tv_sec is always 0.....
  tv->tv_usec = (int32_t)uptime.us;
  tv->tv_sec = (int32_t)uptime.us / 1000000;
  // printf("sec is %d, usec is %d\n",tv->tv_sec,tv->tv_usec);
  return 0;
}

// void exit(int status)
// {
//   if (status == 0)
//   {
//     execve("/bin/nterm", 0, 0);
//   }
//   else
//     halt(status);
// }