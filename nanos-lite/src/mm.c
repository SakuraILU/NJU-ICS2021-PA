#include <memory.h>
#include <proc.h>

static void *pf = NULL;
extern PCB *current;

void *new_page(size_t nr_page)
{
  void *old_pf = pf;
  pf = (void *)((char *)pf + nr_page * PGSIZE);
  return old_pf;
}

#ifdef HAS_VME
static void *pg_alloc(int n)
{
  assert(n >= PGSIZE);
  void *p = new_page(n / PGSIZE);
  return memset(p, 0, n);
}
#endif

void free_page(void *p)
{
  panic("not implement yet");
}

// #define USR_SPACE RANGE(0x40000000, 0x80000000)
/* The brk() system call handler. */
int mm_brk(uintptr_t brk)
{
  // assert(brk >= 0x)
  // f (brk < (uintptr_t)USR_SPACE.start || (brk > (uintptr_t)USR_SPACE.end - USR_STACK_PG_NUM * PGSIZE))
  // {
  //   printf("invalid!\n");
  //   return 0;i
  // }
#define PG_MASK ~0xfff
  if (current->max_brk == 0)
  {
    current->max_brk = (brk & ~PG_MASK) ? ((brk & PG_MASK) + PGSIZE) : brk;
    printf("first malloc is at %p\n", (void *)current->max_brk);
    return 0;
  }

  for (; current->max_brk < brk; current->max_brk += PGSIZE)
  {
    // printf("malloc new space for virtual %p, brk is %p\n", (void *)current->max_brk, (void *)brk);
#ifdef USR_SPACE_ENABLE
    // printf("malloc new space %p for virtual %p\n", pg_p, (void *)i);
    // map(&current->as, (void *)current->max_brk, pg_alloc(PGSIZE), PGSIZE);
    map(&current->as, (void *)current->max_brk, pg_alloc(PGSIZE), PTE_R | PTE_W | PTE_X);
#endif
  }

  // printf("finished malloc\n");
  return 0;
}

void init_mm()
{
  pf = (void *)ROUNDUP(heap.start, PGSIZE);
  Log("free physical pages starting from %p", pf);

#ifdef HAS_VME
  vme_init(pg_alloc, free_page);
#endif
}
