.globl  _start
_start:
  move s0, zero
  move a0, sp
  jal call_main
