#include <common.h>
#include "syscall.h"

// enum {
//     EVENT_NULL = 0,
//     EVENT_YIELD, EVENT_SYSCALL, EVENT_PAGEFAULT, EVENT_ERROR,
//     EVENT_IRQ_TIMER, EVENT_IRQ_IODEV,
// }
void do_syscall(Context *c);

static Context* do_event(Event e, Context* c) {
  // Log("event is %d\n",e.event);
  switch (e.event) {
    case EVENT_YIELD: printf("YEILD event happend!\n"); break;
    case EVENT_SYSCALL: do_syscall(c); break;
    default: panic("Unhandled event ID = %d", e.event);
  }
  return c;
}

void init_irq(void) {
  Log("Initializing interrupt/exception handler...");
  cte_init(do_event);
}
