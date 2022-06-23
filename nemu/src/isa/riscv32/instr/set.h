def_EHelper(sltiu) {
    rtl_setrelopi(s, RELOP_LTU, id_dest->preg, id_src1->preg, c_sext(id_src2->imm, 20));
}

def_EHelper(sltu) {
    rtl_setrelop(s, RELOP_LTU, id_dest->preg, id_src1->preg, id_src2->preg);
}

def_EHelper(slt) {
    rtl_setrelop(s, RELOP_LT, id_dest->preg, id_src1->preg, id_src2->preg);
}

def_EHelper(slti) {
    rtl_setrelopi(s, RELOP_LT, id_dest->preg, id_src1->preg, c_sext(id_src2->imm, 20));
}
