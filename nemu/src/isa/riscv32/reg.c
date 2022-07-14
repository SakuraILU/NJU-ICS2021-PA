#include <isa.h>
#include "local-include/reg.h"

#define NR_REGS ARRLEN(regs) //sizeof(regs)/sizeof(regs[0])
#define NR_CSRS ARRLEN(csrs) //sizeof(csrs)/sizeof(csrs[0])

const char *regs[] = {
  "$0", "ra", "sp", "gp", "tp", "t0", "t1", "t2",
  "s0", "s1", "a0", "a1", "a2", "a3", "a4", "a5",
  "a6", "a7", "s2", "s3", "s4", "s5", "s6", "s7",
  "s8", "s9", "s10", "s11", "t3", "t4", "t5", "t6"
};

const char *csrs[] = {
  "mtvec","mepc","mstatus","mcause"
};

void isa_reg_display() {
  printf("%-15s%-17s%-15s\n", "Registers", "Hexadecimal", "Decimal");
  for(int i = 0; i < NR_REGS; ++i){
    printf("%-15s0x%-15x%-15d\n",regs[i], cpu.gpr[i]._32, cpu.gpr[i]._32);
  }
  printf("%-15s0x%-15x%-15d\n","pc", cpu.pc, cpu.pc);
  for(int i = 0; i < NR_CSRS; ++i){
    printf("%-15s0x%-15x%-15d\n",csrs[i], cpu.csr[i]._32, cpu.csr[i]._32);
  }
}

word_t isa_reg_str2val(const char *s, bool *success) {
  
  *success = false;
  for(int i = 0; i < NR_REGS; ++i ){
    if(strcmp(regs[i], s+1) == 0){ *success = true; return cpu.gpr[i]._32;}
  }
  if(strcmp("pc", s+1) == 0){*success = true; return cpu.pc;}
  for(int i = 0; i < NR_CSRS; ++i){
    if(strcmp(csrs[i], s+1) == 0){ *success = true; return cpu.csr[i]._32;}
  }
  
  return 0;
}

int save_regs(FILE* fp){
  if(fwrite(&cpu, 1, sizeof(cpu), fp) == 0) panic("Fail to save regs in cpu\n");
  return 0;
}

int load_regs(FILE* fp){
  if(fread(&cpu, 1, sizeof(cpu), fp) == 0) panic("Fail to load regs in cpu\n");
  return 0;
}