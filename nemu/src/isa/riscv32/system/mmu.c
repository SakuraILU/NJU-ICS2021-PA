#include <isa.h>
#include <memory/paddr.h>
#include <memory/vaddr.h>
#include <isa.h>

paddr_t isa_mmu_translate(vaddr_t vaddr, int len, int type)
{
  // // {
  // printf("================\n");
  // }
#define SATP_MASK 0X3fffff
#define PG_SHIFT 12
#define PGT1_ID(val) (val >> 22)
#define PGT2_ID(val) ((val & 0x3fffff) >> 12)
#define OFFSET(val) (val & 0xfff)
  word_t va_raw = (uint32_t)vaddr;
  paddr_t *pt_1 = (paddr_t *)guest_to_host((paddr_t)(cpu.csr[REG_SATP]._32 << PG_SHIFT));
  // printf("pt1[id1(vaddr)] is 0x%x\n", pt_1[PGT1_ID(va_raw)]);
  assert(pt_1 != NULL);
  word_t *pt_2 = (word_t *)guest_to_host(pt_1[PGT1_ID(va_raw)]);
  // if (vaddr == 0x7ffffded)

  assert(pt_2 != NULL);
  paddr_t paddr = (paddr_t)((pt_2[PGT2_ID(va_raw)] & (~0xfff)) | OFFSET(va_raw));
  // if (vaddr >= 0x40000000 && vaddr <= 0x8000000)
  // {
  // printf("mmu translate vaddr %p\n", (void *)(long)vaddr);
  // printf("translate ui is 0x%x, pt2 id is %x\n", pt_2[PGT2_ID(va_raw)] >> 12, PGT2_ID(va_raw));
  // printf("mmu translate to paddr %p\n", (void *)(long)paddr);
  // } // printf("vaddr is 0x%x, paddr is 0x%x\n", vaddr, paddr);
  assert(vaddr >= 0x40000000 && vaddr <= 0xa1200000);
  // assert(paddr == vaddr);
  // return MEM_RET_FAIL;
  return paddr;
}
