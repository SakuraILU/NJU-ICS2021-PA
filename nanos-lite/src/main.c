#include <common.h>

void init_mm(void);
void init_device(void);
void init_ramdisk(void);
void init_irq(void);
void init_fs(void);
void init_proc(void);


int main() {
  extern const char logo[];
  printf("%s", logo);
  Log("'Hello World!' from Nanos-lite\n");
  Log("Build time: %s, %s\n", __TIME__, __DATE__);

  init_mm();

  init_device();

  init_ramdisk();

#ifdef HAS_CTE
  init_irq();
#endif

  init_fs();

  init_proc();

  Log("Finish initialization\n");

#ifdef HAS_CTE
  yield();
#endif

  panic("Should not reach here\n");
}
