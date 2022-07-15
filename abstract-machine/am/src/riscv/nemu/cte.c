#include <am.h>
#include <riscv/riscv.h>
#include <klib.h>

static Context *(*user_handler)(Event, Context *) = NULL;

void display_context(Context *c)
{
  for (int i = 0; i < sizeof(c->gpr) / sizeof(c->gpr[0]); ++i)
    printf("x%d is %d\n", i, c->gpr[i]);
  printf("mcause, mstatus, mepc is %p, %p, %p\n", c->mcause, c->mstatus, c->mepc);
}

Context *__am_irq_handle(Context *c)
{
  // printf("cause to interrupt is %d\n",c->GPR1);
  // display_context(c);
  if (user_handler)
  {
    // printf("the exception cause id is %d\n", c->mcause);
    Event ev = {0};

    switch (c->mcause)
    {
    case -1:
      ev.event = EVENT_YIELD;
      break;
    case 0 ... 19:
      ev.event = EVENT_SYSCALL;
      break;
    default:
      ev.event = EVENT_ERROR;
      break;
    }

    // printf("event id is %d\n",ev.event);
    c = user_handler(ev, c);
    assert(c != NULL);
  }
  // printf("ret from interrupt is %d\n",c->GPRx);

  return c;
}

extern void __am_asm_trap(void);

bool cte_init(Context *(*handler)(Event, Context *))
{
  // initialize exception entry
  asm volatile("csrw mtvec, %0"
               :
               : "r"(__am_asm_trap));

  // register event handler
  user_handler = handler;

  return true;
}

Context *kcontext(Area kstack, void (*entry)(void *), void *arg)
{
  return NULL;
}

void yield()
{
  asm volatile("li a7, -1; ecall");
}

bool ienabled()
{
  return false;
}

void iset(bool enable)
{
}
