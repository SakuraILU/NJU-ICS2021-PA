#ifndef __NDL_H__
#define __NDL_H__

#include <stdint.h>
#include <assert.h>

#ifdef __cplusplus
extern "C" {
#endif

int NDL_Init(uint32_t flags);
void NDL_Quit();
uint32_t NDL_GetTicks();
void NDL_OpenCanvas(int *w, int *h);
int NDL_PollEvent(char *buf, int len);
void NDL_DrawRect(uint32_t *pixels, int x, int y, int w, int h);
void NDL_OpenAudio(int freq, int channels, int samples);
void NDL_CloseAudio();
int NDL_PlayAudio(void *buf, int len);
int NDL_QueryAudio();

#define panic(format,...) {printf("\e[1;35m[%s, %d, %s] \e[0m " format "\n",__FILE__, __LINE__,__func__, ## __VA_ARGS__); assert(0);}while(0)
#define TODO() panic("PLEASE IMPLEMENT ME!\n")

#ifdef __cplusplus
}
#endif

#endif
