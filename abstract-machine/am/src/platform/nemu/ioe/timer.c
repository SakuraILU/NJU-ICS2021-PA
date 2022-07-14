#include <am.h>
#include <nemu.h>
#include <klib.h>

static uint64_t sys_init_time;

void __am_timer_init() {
  sys_init_time =(  ( (uint64_t)inl(RTC_ADDR+4) << 32 ) + (uint64_t)inl(RTC_ADDR) );
  // printf("time is %d\n",(int)sys_init_time/1000000);
}

void __am_timer_uptime(AM_TIMER_UPTIME_T *uptime) {
  // printf("time is %d\n", (int)uptime->us);
  uptime->us = (  ( (uint64_t)inl(RTC_ADDR + 4) << 32 ) + (uint64_t)inl(RTC_ADDR)  ) - sys_init_time; 
}

void __am_timer_rtc(AM_TIMER_RTC_T *rtc) {
  rtc->second = 0;
  rtc->minute = 0;
  rtc->hour   = 0;
  rtc->day    = 0;
  rtc->month  = 0;
  rtc->year   = 1900;
}
