#include <am.h>
#include <klib.h>
#include <klib-macros.h>

#if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)
static unsigned long int next = 1;

int rand(void) {
  // RAND_MAX assumed to be 32767
  next = next * 1103515245 + 12345;
  return (unsigned int)(next/65536) % 32768;
}

void srand(unsigned int seed) {
  next = seed;
}

int abs(int x) {
  return (x < 0 ? -x : x);
}

int atoi(const char* nptr) {
  int x = 0;
  while (*nptr == ' ') { nptr ++; }
  while (*nptr >= '0' && *nptr <= '9') {
    x = x * 10 + *nptr - '0';
    nptr ++;
  }
  return x;
}

#define HEAP_SIZE 1000000
static uint8_t mem[HEAP_SIZE] = {0};
typedef struct heap
{
  uint8_t *start;
  uint8_t *end;
  uint8_t *hbrk;
  uint8_t *mem;
  uint32_t size;
  /* data */
} Heap;
static Heap mem_heap = {
  .mem = mem,
  .start = mem,
  .hbrk = mem,
  .size = HEAP_SIZE,
  .end = mem + HEAP_SIZE -1,
};
void *malloc(size_t size) {
  // On native, malloc() will be called during initializaion of C runtime.
  // Therefore do not call panic() here, else it will yield a dead recursion:
  //   panic() -> putchar() -> (glibc) -> malloc() -> panic()
#if !(defined(__ISA_NATIVE__) && defined(__NATIVE_USE_KLIB__))
  uint8_t *old_hbrk = mem_heap.hbrk;
  mem_heap.hbrk += size;

  printf("the alloced heap of [%x,%x] adress is %x with size %d", mem_heap.start, mem_heap.end ,old_hbrk, size);
  printf("\n");
  assert( (mem_heap.hbrk >= mem_heap.start) && (mem_heap.hbrk < mem_heap.end) );
#endif
  return (void*)old_hbrk;
}

void free(void *ptr) {
}

#endif
