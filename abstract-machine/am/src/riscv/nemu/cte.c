#include <am.h>
#include <riscv/riscv.h>
#include <klib.h>

static Context* (*user_handler)(Event, Context*) = NULL;

void display_context(Context *c){
  for(int i = 0; i < sizeof(c->gpr)/sizeof(c->gpr[0]); ++i)
    printf("x%d is %d\n", i, c->gpr[i]);
  printf("mcause, mstatus, mepc is %p, %p, %p\n", c->mcause, c->mstatus, c->mepc);
}

Context* __am_irq_handle(Context *c) {
  // display_context(c);
  if (user_handler) {
    // printf("the event id is %d", c->mcause);
    Event ev = {0};
    switch (c->mcause) {
      case -1: ev.event = EVENT_YIELD; break;
      default: ev.event = EVENT_ERROR; break;
    }

    c = user_handler(ev, c);
    assert(c != NULL);
  }

  return c;
}

extern void __am_asm_trap(void);

bool cte_init(Context*(*handler)(Event, Context*)) {
  // initialize exception entry
  asm volatile("csrw mtvec, %0" : : "r"(__am_asm_trap));

  // register event handler
  user_handler = handler;

  return true;
}

Context *kcontext(Area kstack, void (*entry)(void *), void *arg) {
  return NULL;
}

void yield() {
  //11: Environment call from M-mode
  asm volatile("li a7, -1; ecall");
}

bool ienabled() {
  return false;
}

void iset(bool enable) {
}
