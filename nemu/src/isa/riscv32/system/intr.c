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
  if (cpu.csr[REG_MSTATUS]._32 & MSTATUS_MIE)
    cpu.csr[REG_MSTATUS]._32 |= MSTATUS_MPIE;
  else
    cpu.csr[REG_MSTATUS]._32 &= (~MSTATUS_MPIE);

  cpu.csr[REG_MSTATUS]._32 &= (~MSTATUS_MIE);

  // Log("raise interrupt! to %p from %p", (void *)(long)cpu.csr[REG_MTVEC]._32, (void *)(long)epc);

  return cpu.csr[REG_MTVEC]._32;
}

word_t isa_query_intr()
{
  if (cpu.INTR && (cpu.csr[REG_MSTATUS]._32 & MSTATUS_MIE))
  {
    cpu.INTR = false;
    // printf("cpu.INTR is %d\n", cpu.INTR);
    return IRQ_TIMER;
  }
  return INTR_EMPTY;
}
