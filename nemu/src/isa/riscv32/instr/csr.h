

#define CSRS_LIST(f) f(MTVEC), f(MEPC), f(MSTATUS), f(MCAUSE)
#define REGNAME(name) concat(REG_, name)
enum
{
  CSRS_LIST(REGNAME)
};
#define CASE(instr_id, enum_id) \
  case instr_id:                \
  {                             \
    return enum_id;             \
  }

static inline uint8_t csr_id_instr2array(uint32_t instr_id)
{
  // Log("the instr_id is %x\n", instr_id);
  switch (instr_id)
  {
    CASE(0x305, REG_MTVEC);
    CASE(0x342, REG_MCAUSE);
    CASE(0x300, REG_MSTATUS);
    CASE(0x341, REG_MEPC);
  default:
    Assert(0, "NOT a Valid CSR REGISTER!");
    return -1;
  }
}

void difftest_skip_ref();
#define csr(idx) (cpu.csr[csr_id_instr2array(idx)]._32)
def_EHelper(csrrw)
{
  // difftest_skip_ref();
  rtl_mv(s, s0, &csr(id_src2->imm));
  rtl_mv(s, &csr(id_src2->imm), id_src1->preg);
  rtl_mv(s, id_dest->preg, s0);
}

def_EHelper(csrrs)
{
  // difftest_skip_ref();
  rtl_mv(s, s0, &csr(id_src2->imm));
  rtl_or(s, &csr(id_src2->imm), s0, id_src1->preg);
  rtl_mv(s, id_dest->preg, s0);
}

def_EHelper(ecall)
{
  bool success = false;
  word_t trap_no = isa_reg_str2val("$a7", &success);
  // Log("trap number is %d\n",trap_no);
  // Log("trap number is %d\n", trap_no);
  if (!success)
    Assert(0, "Invalid gpr register!");
  word_t trap_vec = isa_raise_intr(trap_no, s->snpc);
  rtl_j(s, trap_vec);
}

def_EHelper(mret)
{
  // Log("return!\n");
  rtl_j(s, cpu.csr[REG_MEPC]._32);
}