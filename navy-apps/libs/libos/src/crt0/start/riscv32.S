#define GRPx a0

.globl  _start
_start:
  move sp, GRPx
  move s0, zero
  jal call_main
