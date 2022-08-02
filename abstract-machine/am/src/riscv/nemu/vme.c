#include <am.h>
#include <nemu.h>
#include <klib.h>

static AddrSpace kas = {};
static void *(*pgalloc_usr)(int) = NULL;
static void (*pgfree_usr)(void *) = NULL;
static int vme_enable = 0;

static Area segments[] = { // Kernel memory mappings
    NEMU_PADDR_SPACE};

#define USER_SPACE RANGE(0x40000000, 0x80000000)

static inline void set_satp(void *pdir)
{
  uintptr_t mode = 1ul << (__riscv_xlen - 1);
  asm volatile("csrw satp, %0"
               :
               : "r"(mode | ((uintptr_t)pdir >> 12)));
  // printf("store data %p in satp \n", mode | ((uintptr_t)pdir >> 12));
}

static inline uintptr_t get_satp()
{
  uintptr_t satp;
  asm volatile("csrr %0, satp"
               : "=r"(satp));
  return satp << 12;
}

bool vme_init(void *(*pgalloc_f)(int), void (*pgfree_f)(void *))
{
  pgalloc_usr = pgalloc_f;
  pgfree_usr = pgfree_f;

  kas.ptr = pgalloc_f(PGSIZE);

  // printf("create page table at %p\n", kas.ptr);
  int i;
  for (i = 0; i < LENGTH(segments); i++)
  {
    void *va = segments[i].start;
    for (; va < segments[i].end; va += PGSIZE)
    {
      map(&kas, va, va, PTE_R | PTE_W | PTE_X);
    }
  }

  set_satp(kas.ptr);
  vme_enable = 1;

  return true;
}

void protect(AddrSpace *as)
{
  PTE *updir = (PTE *)(pgalloc_usr(PGSIZE));
  as->ptr = updir;
  as->area = USER_SPACE;
  as->pgsize = PGSIZE;
  // map kernel space
  memcpy(updir, kas.ptr, PGSIZE);
}

void unprotect(AddrSpace *as)
{
}

void __am_get_cur_as(Context *c)
{
  if (c->pdir != NULL)
    c->pdir = (vme_enable ? (void *)get_satp() : NULL);
}

void __am_switch(Context *c)
{
  // printf("change ctx\n");
  if (vme_enable && c->pdir != NULL)
  {
    // printf("pte is at %p after change\n", c);
    set_satp(c->pdir);
  }
}

void map(AddrSpace *as, void *va, void *pa, int prot)
{
#define PGT1_ID(val) (val >> 22)
#define PGT2_ID(val) ((val & 0x3fffff) >> 12)
  uint32_t pa_raw = (uint32_t)pa;
  uint32_t va_raw = (uint32_t)va;
  uint32_t **pt_1 = (uint32_t **)as->ptr;
  if (pt_1[PGT1_ID(va_raw)] == NULL)
    pt_1[PGT1_ID(va_raw)] = (uint32_t *)pgalloc_usr(PGSIZE);

  uint32_t *pt_2 = pt_1[PGT1_ID(va_raw)];
  if (pt_2[PGT2_ID(va_raw)] == 0)
    pt_2[PGT2_ID(va_raw)] = (pa_raw & (~0xfff)) | prot;
  else
  {
    Assert(0, "remap virtual address\n!");
  }
  // printf("map vrirtual address %p to physical address %p, pt2 id is %p, store addr %p\n", va_raw, pa_raw, PGT2_ID(va_raw), pt_2[PGT2_ID(va_raw)] >> 12);
}

#include <klib.h>
Context *ucontext(AddrSpace *as, Area kstack, void *entry)
{

  Context *ctx = (Context *)((uint8_t *)(kstack.end) - sizeof(Context));
  memset(ctx, 0, sizeof(ctx));

  ctx->mepc = (uintptr_t)entry;
  assert(ctx->mepc >= 0x40000000 && ctx->mepc <= 0x88000000);

  ctx->mstatus = 0x1800 | MSTATUS_MPIE;
  ctx->gpr[0] = 0;

  ctx->mscratch = (uintptr_t)kstack.end;
  // ctx->GPRx = (uintptr_t)(heap.end);
  // printf("the heap end is %p\n", heap.end);

  return ctx;
}
