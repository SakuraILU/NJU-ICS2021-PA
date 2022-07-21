#include <isa.h>

word_t isa_raise_intr(word_t NO, vaddr_t epc)
{
  /* TODO: Trigger an interrupt/exception with ``NO''.
   * Then return the address of the interrupt/exception vector.
   */
  cpu.csr[REG_MEPC]._32 = epc;
  cpu.csr[REG_MCAUSE]._32 = NO;
  // cpu.pc = cpu.csr[REG_MTVEC]._32;
  // Log("cpu.pc is %x", cpu.pc);
  return cpu.csr[REG_MTVEC]._32;
}

word_t isa_query_intr()
{
  return INTR_EMPTY;
}
