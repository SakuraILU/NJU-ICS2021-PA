#include <isa.h>

void dev_raise_intr()
{
    cpu.INTR = true;
    // printf("cpu INTR is %d\n", cpu.INTR);
}
