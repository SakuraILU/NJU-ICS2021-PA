#include <am.h>
#include <riscv/riscv.h>
#include <klib.h>
#include <nemu.h>

static Context *(*user_handler)(Event, Context *) = NULL;
void __am_get_cur_as(Context *c);
void __am_switch(Context *c);

void display_context(Context *c)
{
  for (int i = 0; i < sizeof(c->gpr) / sizeof(c->gpr[0]); ++i)
    printf("x%p is %p\n", i, c->gpr[i]);
  printf("mcause, mstatus, mepc is %p, %p, %p\n", c->mcause, c->mstatus, c->mepc);
}

Context *__am_irq_handle(Context *c)
{
  // __am_get_cur_as(c);
  // display_context(c);
  // printf("pte is at %p\n", c->pdir);
  // printf("cause to interrupt is %d\n", c->GPR1);
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
    case 0x80000007:
      ev.event = EVENT_IRQ_TIMER;
      break;
    default:
      ev.event = EVENT_ERROR;
      break;
    }

    // printf("event id is %d\n",ev.event);
    // printf("here\n");
    c = user_handler(ev, c);
    // printf("===============");
    // printf("mepc is %p\n", c->mepc);
    // printf("c is at %p\n",c);
    assert(c != NULL);
  }
  // printf("ret from interrupt is %p\n", c->GPRx);
  // ((void (*)())c->mepc)();
  // c->gpr[0] = 0;
  // printf("pte is at %p after change\n", c->mepc);
  // printf("sp is %p\n", c->gpr[2]);
  // display_context(c);
  assert(c->mepc >= 0x40000000 && c->mepc <= 0x88000000);
  // __am_switch(c);
  // printf("entry is %p\n", c->mepc);
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
  // return NULL;
  Context *ctx = (Context *)((uint8_t *)(kstack.end) - sizeof(Context));
  memset(ctx, 0, sizeof(ctx));

  ctx->gpr[0] = 0;
  ctx->mepc = (uintptr_t)entry;
  ctx->mstatus = 0x1800 | MSTATUS_MPIE;
  ctx->GPRx = (uintptr_t)arg;
  // ctx->pdir = NULL;
  ctx->mscratch = 0;
  // printf("args is %d\n", *((int *)ctx->gpr[10]));
  // while (1)
  // {
  //   /* code */
  // }

  // printf("ctx gpr 0 is %d\n", ctx->gpr[0]);
  return ctx;
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
