#include <unistd.h>
#include <stdio.h>
#include <sys/time.h>
#include <NDL.h>
int main() {
  // struct timeval timeval;
  // struct timezone timezone;
  printf("here\n");
  // int res = gettimeofday(&timeval, &timezone);
  NDL_Init(0);
  uint32_t sec = 0;
  uint32_t usec = 0;
  while(1) {
    while(usec/100000 < sec){
      usec = NDL_GetTicks();
      printf("usec is %d\n",usec);
    }
    usec = NDL_GetTicks();
    printf("some setence~~\n----------------\n");
    sec += 5;
  }
  return 0;
}
