#include <cpu/decode.h>
#include "../local-include/rtl.h"

#define INSTR_LIST(f) f(lui) f(lw) f(sw) f(inv) f(nemu_trap) \
                      f(addi) f(auipc) f(jal) f(jalr) \
                      f(add) f(sub) f(sltiu) f(beq) f(bne) \
                      f(sltu) f(xor) f(or) f(sh) f(srai) f(lbu) f(andi) f(sll) f(and) f(xori) f(sb) \
                      f(bge) \
                      f(mul) f(div) f(rem) \
                      f(blt) f(slt) \
                      f(lh) f(lhu) f(slli) f(srli) \
                      f(mulh) \
                      f(sra) f(srl)\
                      f(bltu) \
                      f(bgeu) \
                      f(divu) f(lb) f(ori) f(remu) f(mulhu)\
                      f(csrrw) f(ecall) f(csrrs) f(mret) \
                      f(slti) \

def_all_EXEC_ID();
