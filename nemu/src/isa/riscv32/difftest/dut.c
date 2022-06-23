#include <isa.h>
#include <cpu/difftest.h>
#include "../local-include/reg.h"

#define NR_REGS sizeof(cpu.gpr)/sizeof(cpu.gpr[0])
const char *reg[] = {
  "$0", "ra", "sp", "gp", "tp", "t0", "t1", "t2",
  "s0", "s1", "a0", "a1", "a2", "a3", "a4", "a5",
  "a6", "a7", "s2", "s3", "s4", "s5", "s6", "s7",
  "s8", "s9", "s10", "s11", "t3", "t4", "t5", "t6"
};

bool isa_difftest_checkregs(CPU_state *ref_r, vaddr_t pc) {
  bool is_same = difftest_check_reg("pc", cpu.pc, ref_r->pc, cpu.pc);
  for(int i = 0; i < NR_REGS; i++){
    is_same = difftest_check_reg(reg[i] , cpu.pc, ref_r->gpr[i]._32, cpu.gpr[i]._32); 
    if(!is_same) return false;
  }
  return is_same;
}

void isa_difftest_attach() {
}
