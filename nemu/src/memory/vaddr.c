#include <isa.h>
#include <memory/paddr.h>

word_t vaddr_ifetch(vaddr_t addr, int len)
{
  if (isa_mmu_check(addr, len, MEM_TYPE_IFETCH))
    return paddr_read(isa_mmu_translate(addr, len, MEM_TYPE_IFETCH), len);

  return paddr_read(addr, len);
}

word_t vaddr_read(vaddr_t addr, int len)
{
  if (isa_mmu_check(addr, len, MEM_TYPE_READ))
    return paddr_read(isa_mmu_translate(addr, len, MEM_TYPE_READ), len);

  return paddr_read(addr, len);
}

void vaddr_write(vaddr_t addr, int len, word_t data)
{
  if (isa_mmu_check(addr, len, MEM_TYPE_WRITE))
    return paddr_write(isa_mmu_translate(addr, len, MEM_TYPE_WRITE), len, data);

  paddr_write(addr, len, data);
}
