#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/time.h>
#include <assert.h>
#include <fcntl.h>

#define IS_NUM(ch) (ch >= '0' && ch <= '9')

static int evtdev = -1;
static int fbdev = -1;
static int dispinfo_dev = -1;
static int screen_w = 0, screen_h = 0;

typedef struct size
{
  int w;
  int h;
} Size;
Size disp_size;

// static void get_disp_size()
// {
// #define DISPINFO_LEN 64
//   char buf[DISPINFO_LEN];
//   assert(read(dispinfo_dev, buf, DISPINFO_LEN) != 0);
//   size_t i = 0;
//   // printf("buf is %s\n", buf);
//   for (; buf[i] != '\n'; ++i)
//   {
//     if (IS_NUM(buf[i]))
//       disp_size.w = disp_size.w * 10 + (buf[i] - '0');
//   }
//   ++i;
//   for (; buf[i] != '\n'; ++i)
//   {
//     if (IS_NUM(buf[i]))
//       disp_size.h = disp_size.h * 10 + (buf[i] - '0');
//   }
//   assert(disp_size.w > 0 && disp_size.h <= 800);
//   assert(disp_size.h > 0 && disp_size.h <= 640);
// }

// return ms
uint32_t NDL_GetTicks()
{
  static struct timeval timeval;
  static struct timezone timezone;
  int ret = gettimeofday(&timeval, &timezone);
  return timeval.tv_usec / 1000;
}

int NDL_PollEvent(char *buf, int len)
{
  buf[0] = '\0';
  assert(evtdev != -1);
  int ret = read(evtdev, buf, len);
  return ret;
}

void NDL_OpenCanvas(int *w, int *h)
{
  if (*w == 0 && *h == 0)
  {
    *w = disp_size.w;
    *h = disp_size.h;
  }

  if (getenv("NWM_APP"))
  {
    int fbctl = 4;
    fbdev = 5;
    screen_w = *w;
    screen_h = *h;
    char buf[64];
    int len = sprintf(buf, "%d %d", screen_w, screen_h);
    // let NWM resize the window and create the frame buffer
    write(fbctl, buf, len);
    while (1)
    {
      // 3 = evtdev
      int nread = read(3, buf, sizeof(buf) - 1);
      if (nread <= 0)
        continue;
      buf[nread] = '\0';
      if (strcmp(buf, "mmap ok") == 0)
        break;
    }

    close(fbctl);
  }
}

void NDL_DrawRect(uint32_t *pixels, int x, int y, int w, int h)
{
  if (w == 0 && h == 0)
  {
    w = disp_size.w;
    h = disp_size.h;
  }
  assert(w > 0 && w <= disp_size.w);
  assert(h > 0 && h <= disp_size.h);

  // write(1, "here\n", 10);
  // printf("draw [%d, %d] to [%d, %d]\n", w, h, x, y);
  for (size_t row = 0; row < h; ++row)
  {
    // printf("draw row %d with len %d\n", row, w);
    lseek(fbdev, x + (y + row) * disp_size.w, SEEK_SET);
    // printf("pixels pos %p with len %d\n",pixels + row * w, w);
    write(fbdev, pixels + row * w, w);
    // printf("draw row %d with len %d\n", row, w);
  }
  write(fbdev, 0, 0);
}

void NDL_OpenAudio(int freq, int channels, int samples)
{
}

void NDL_CloseAudio()
{
}

int NDL_PlayAudio(void *buf, int len)
{
  return 0;
}

int NDL_QueryAudio()
{
  return 0;
}

int NDL_Init(uint32_t flags)
{
  if (getenv("NWM_APP"))
  {
    evtdev = 3;
  }
  evtdev = open("/dev/events", 0, 0);
  fbdev = open("/dev/fb", 0, 0);
  dispinfo_dev = open("/proc/dispinfo", 0, 0);

  // get_disp_size();
  FILE *fp = fopen("/proc/dispinfo", "r");
  fscanf(fp, "WIDTH:%d\nHEIGHT:%d\n", &disp_size.w, &disp_size.h);
  // printf("disp size is %d,%d\n", disp_size.w, disp_size.h);
  assert(disp_size.w >= 400 && disp_size.w <= 800);
  assert(disp_size.h >= 300 && disp_size.h <= 640);
  fclose(fp);
  return 0;
}

void NDL_Quit()
{
  close(evtdev);
  close(dispinfo_dev);
}
