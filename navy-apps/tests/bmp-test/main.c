#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <NDL.h>
#include <BMP.h>

int main()
{
  // printf("Test ends! Spinning...\n");
  NDL_Init(0);
  int w, h;
  void *bmp = BMP_Load("/share/pictures/projectn.bmp", &w, &h);
  printf("bmp pos is at [%p, %p]\n", bmp, (int *)bmp + w * h);
  assert(bmp);
  NDL_OpenCanvas(&w, &h);
  NDL_DrawRect(bmp, 0, 0, w, h);
  NDL_Quit();
  free(bmp);
  printf("Test ends! Spinning...\n");
  while (1)
    ;
  return 0;
}
