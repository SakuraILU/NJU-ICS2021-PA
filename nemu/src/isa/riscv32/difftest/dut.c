#include <isa.h>
#include <cpu/difftest.h>
#include "../local-include/reg.h"

#define NR_REGS sizeof(cpu.gpr)/sizeof(cpu.gpr[0])

bool isa_difftest_checkregs(CPU_state *ref_r, vaddr_t pc) {
  bool is_same = (cpu.pc == ref_r->pc);
  for(int i = 0; i < NR_REGS; i++){
    if(!is_same) break;
    is_same = (cpu.gpr[i]._32 == ref_r->gpr[i]._32);
  }
  return is_same;
}

void isa_difftest_attach() {
}
