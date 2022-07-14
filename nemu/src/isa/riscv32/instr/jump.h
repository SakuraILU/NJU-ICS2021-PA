def_EHelper(jal)
{
  rtl_addi(s, id_dest->preg, &(s->pc), 4);
  // important: need to fresh the pc register
  rtl_addi(s, s0, &(s->pc), c_sext(id_src1->imm, 11));
  rtl_j(s, *s0);
}

def_EHelper(jalr)
{
  rtl_addi(s, s0, &(s->pc), 4);

  rtl_addi(s, s1, id_src1->preg, c_sext(id_src2->imm, 20));
  rtl_andi(s, s1, s1, -2); //(sword_t)-2 is (word_t)0xfffffffe
  rtl_j(s, *s1);
  rtl_mv(s, id_dest->preg, s0);
}

def_EHelper(beq)
{
  rtl_setrelop(s, RELOP_EQ, s0, id_src1->preg, id_src2->preg);
  if (*s0)
  {
    rtl_addi(s, s0, &(s->pc), c_sext(id_dest->imm, 19));
    rtl_j(s, *s0);
  }
}

def_EHelper(bne)
{
  rtl_setrelop(s, RELOP_NE, s0, id_src1->preg, id_src2->preg);
  if (*s0)
  {
    rtl_addi(s, s0, &(s->pc), c_sext(id_dest->imm, 19));
    rtl_j(s, *s0);
  }
}

def_EHelper(bge)
{
  rtl_setrelop(s, RELOP_GE, s0, id_src1->preg, id_src2->preg);
  if (*s0)
  {
    rtl_addi(s, s0, &(s->pc), c_sext(id_dest->imm, 19));
    rtl_j(s, *s0);
  }
}

def_EHelper(blt)
{
  rtl_setrelop(s, RELOP_LT, s0, id_src1->preg, id_src2->preg);
  if (*s0)
  {
    rtl_addi(s, s0, &(s->pc), c_sext(id_dest->imm, 19));
    rtl_j(s, *s0);
  }
}

def_EHelper(bltu)
{
  rtl_setrelop(s, RELOP_LTU, s0, id_src1->preg, id_src2->preg);
  if (*s0)
  {
    rtl_addi(s, s0, &(s->pc), c_sext(id_dest->imm, 19));
    rtl_j(s, *s0);
  }
}

def_EHelper(bgeu)
{
  rtl_setrelop(s, RELOP_GEU, s0, id_src1->preg, id_src2->preg);
  if (*s0)
  {
    rtl_addi(s, s0, &(s->pc), c_sext(id_dest->imm, 19));
    rtl_j(s, *s0);
  }
}