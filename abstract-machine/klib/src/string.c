#include <klib.h>
#include <klib-macros.h>
#include <stdint.h>
#include <stdio.h>
#if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)

size_t strlen(const char *s) {
  size_t cnt = 0, i = 0;
  while(s[i++] != '\0'){
    cnt ++;
  }
  
  return cnt;
}

char *strcpy(char *dst, const char *src) {
  size_t i = 0;
  for(; src[i] != '\0';++i){
    dst[i] = src[i];
  }
  dst[i] = '\0';
  return dst;
}

char *strncpy(char *dst, const char *src, size_t n) {
  size_t i = 0;
  for(; (i < n) && (src[i]!='\0') ; ++i){
    dst[i] = src[i];
  }  for(;i < n; i++){
    dst[i] = '\0';
  }
  return dst;
}

char *strcat(char *dst, const char *src) {
  size_t pos_dst = strlen(dst);
  size_t pos_src = 0;
  for(; src[pos_src] != '\0'; pos_src++, pos_dst++)
    dst[pos_dst] = src[pos_src];
  dst[pos_dst] = '\0';

  return dst;
}

char *strncat(char *dst, const char *src, size_t n) {
  size_t pos_dst = strlen(dst);
  size_t pos_src = 0;
  for(; (pos_src) < n && (src[pos_src] != '\0'); pos_src++, pos_dst++)
    dst[pos_dst] = src[pos_src];
  dst[pos_dst] = '\0';

  return dst;
}

int strcmp(const char *s1, const char *s2) {
  size_t i = 0;
  for(; s1[i] != '\0' && s2[i] != '\0'; ++i){
    if(s1[i] > s2[i]) return 1;
    else if(s1[i] == s2[i]) continue;
    else return -1;
  }

  if(s1[i] == '\0' && s2[i] == '\0') return 0;
  else if(s1[i] == '\0') return -1;
  else return 1;
}

int strncmp(const char *s1, const char *s2, size_t n) {
  size_t i = 0;
  for(; i < n || (s1[i] != '\0' && s2[i] != '\0'); ++i){
    if(s1[i] > s2[i]) return 1;
    else if(s1[i] == s2[i]) continue;
    else return -1;
  }

  if((s1[i] != '\0' && s2[i] != '\0') || (s1[i] == '\0' && s2[i] == '\0')) return 0;
  else if(s1[i] == '\0') return -1;
  else return 1;
}

void *memset(void *s, int c, size_t n) {
  for(size_t i = 0; i < n; ++i){
    *((char*)s + i) = (char)c;
  }
  return s;
}

void *memmove(void *dst, const void *src, size_t n) {
  char tmp[n];
  for(size_t i = 0; i < n; ++ i)
    tmp[i] = *((char*)src + i);
    
  for(size_t i = 0; i < n; ++ i)
    *((char*)dst + i ) = tmp[i];
  
  return dst;
}

void *memcpy(void *out, const void *in, size_t n) {
  for(size_t i = 0; i < n; ++i)
    *((char*)out + i) = *((char*)in + i);
  return out;
}

int memcmp(const void *s1, const void *s2, size_t n) {
  for(int i = 0; i < n; ++i){
    uint8_t val1 = *((uint8_t*)s1 + i);
    uint8_t val2 = *((uint8_t*)s2 + i);
    if(val1 > val2) return 1;
    else if(val1 == val2) continue;
    else return -1;
  }
  return 0;
}

#endif
