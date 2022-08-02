#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
extern char end;
int main()
{
  // char buf[32];
  // sprintf(buf, "pbrk is %p\n", (void *)&end);
  // write(1, "Hello World!\n", 13);
  int i = 0;
  volatile int j = 0;
  // for (int i = 0; i < 1000; ++i)
  // {
  //   // printf("sbrk is at %p\n", sbrk(0));
  //   void *p = (void *)malloc(10 << 12);
  //   // for (int j = 0; j < 300; ++j)
  //   printf("malloc pos 0x%p\n", p);
  //   free(p);
  // }
  while (1)
  {
    j++;
    if (j == 1000)
    {
      // write(1, buf, 20);
      // write(1, "Hello World!\n", 13);
      // char buf[60];
      // sprintf(buf, "Hello World from Navy-apps for the %dth time!\n", i++);
      // write(1, buf, 60);
      printf("Hello World from Navy-apps for the %dth time!\n", i++);
      j = 0;
    }
  }
  return 0;
}
