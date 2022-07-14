def_EHelper(lui)
{
  rtl_li(s, ddest, c_sext(id_src1->imm, 0));
}

def_EHelper(addi)
{
  rtl_addi(s, id_dest->preg, id_src1->preg, c_sext(id_src2->imm, 20));
}

def_EHelper(auipc)
{
  rtl_addi(s, id_dest->preg, &(s->pc), c_sext(id_src1->imm, 0));
}

def_EHelper(add)
{
  rtl_add(s, id_dest->preg, id_src1->preg, id_src2->preg);
}

def_EHelper(sub)
{
  rtl_sub(s, id_dest->preg, id_src1->preg, id_src2->preg);
}

def_EHelper(xor)
{
  rtl_xor(s, id_dest->preg, id_src1->preg, id_src2->preg);
}

def_EHelper(or)
{
  rtl_or(s, id_dest->preg, id_src1->preg, id_src2->preg);
}

def_EHelper(srai)
{
  rtl_srai(s, id_dest->preg, id_src1->preg, id_src2->imm);
}

def_EHelper(andi)
{
  rtl_andi(s, id_dest->preg, id_src1->preg, c_sext(id_src2->imm, 20));
}

def_EHelper(sll)
{
  rtl_sll(s, id_dest->preg, id_src1->preg, id_src2->preg);
}

def_EHelper(and)
{
  rtl_and(s, id_dest->preg, id_src1->preg, id_src2->preg);
}

def_EHelper(xori)
{
  rtl_xori(s, id_dest->preg, id_src1->preg, c_sext(id_src2->imm, 20));
}

def_EHelper(mul)
{
  rtl_mulu_lo(s, id_dest->preg, id_src1->preg, id_src2->preg);
}

def_EHelper(div)
{
  if (!*(id_src2->preg))
    // *(id_dest->preg) = -1;
    panic("divided by 0");
  else
    rtl_divs_q(s, id_dest->preg, id_src1->preg, id_src2->preg);
}

def_EHelper(rem)
{
  if (!*(id_src2->preg))
    *(id_dest->preg) = -1;
  else
    rtl_divs_r(s, id_dest->preg, id_src1->preg, id_src2->preg);
}

def_EHelper(slli)
{
  rtl_slli(s, id_dest->preg, id_src1->preg, id_src2->imm);
}

def_EHelper(srli)
{
  rtl_srli(s, id_dest->preg, id_src1->preg, id_src2->imm);
}

def_EHelper(mulh)
{
  rtl_muls_hi(s, id_dest->preg, id_src1->preg, id_src2->preg);
}

def_EHelper(sra)
{
  rtl_sra(s, id_dest->preg, id_src1->preg, id_src2->preg);
}

def_EHelper(srl)
{
  rtl_srl(s, id_dest->preg, id_src1->preg, id_src2->preg);
}

def_EHelper(divu)
{
  // Log("src1 is %d, src2 is %d",*(id_src1->preg),*(id_src2->preg));
  if (!*(id_src2->preg))
    *(id_dest->preg) = -1;
  // panic("divided by 0");
  else
    rtl_divu_q(s, id_dest->preg, id_src1->preg, id_src2->preg);
}

def_EHelper(ori)
{
  rtl_ori(s, id_dest->preg, id_src1->preg, c_sext(id_src2->imm, 20));
}

def_EHelper(remu)
{
  if (!*(id_src2->preg))
    *(id_dest->preg) = -1;
  // panic("divided by 0");
  else
    rtl_divu_r(s, id_dest->preg, id_src1->preg, id_src2->preg);
}

def_EHelper(mulhu)
{
  rtl_mulu_hi(s, id_dest->preg, id_src1->preg, id_src2->preg);
}
