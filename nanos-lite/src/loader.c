#include <proc.h>
#include <elf.h>
#include <klib.h>
#include <fs.h>

#ifdef __LP64__
#define Elf_Ehdr Elf64_Ehdr
#define Elf_Phdr Elf64_Phdr
#else
#define Elf_Ehdr Elf32_Ehdr
#define Elf_Phdr Elf32_Phdr
#endif

#define min(x, y) ((x < y) ? x : y)

extern uint8_t NR_PROC;
// 从ramdisk中`offset`偏移处的`len`字节读入到`buf`中
// size_t ramdisk_read(void *buf, size_t offset, size_t len);

// 把`buf`中的`len`字节写入到ramdisk中`offset`偏移处
// size_t ramdisk_write(const void *buf, size_t offset, size_t len);

// 返回ramdisk的大小, 单位为字节
// size_t get_ramdisk_size();
#define PG_MASK (~0xfff)
#define ISALIGN(vaddr) ((vaddr) == ((vaddr)&PG_MASK))
#define OFFSET(vaddr) ((vaddr) & (~PG_MASK))
#define NEXT_PAGE(vaddr) ((ISALIGN(vaddr)) ? (vaddr) : ((vaddr)&PG_MASK) + PGSIZE)
static uintptr_t loader(PCB *pcb, const char *filename)
{
  Elf_Ehdr ehdr;

  // ramdisk_read(&ehdr, 0, sizeof(ehdr));
  int fd = fs_open(filename, 0, 0);
  assert(fd != -1);

  fs_read(fd, &ehdr, sizeof(ehdr));

  char riscv32_magic_num[] = {0x7f, 0x45, 0x4c, 0x46, 0x01, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
  // printf("magic number is %s\n", (char *)(ehdr.e_ident));
  assert(strcmp((char *)(ehdr.e_ident), riscv32_magic_num) == 0);

  uint32_t entry = ehdr.e_entry;
  uint32_t ph_offset = ehdr.e_phoff;
  uint32_t ph_num = ehdr.e_phnum;

  Elf_Phdr phdr;
  for (int i = 0; i < ph_num; ++i)
  {
    // ramdisk_read(&phdr, ph_offset + i * sizeof(phdr), sizeof(phdr));
    fs_lseek(fd, ph_offset + i * sizeof(phdr), SEEK_SET);
    fs_read(fd, &phdr, sizeof(phdr));
    if (phdr.p_type != PT_LOAD)
      continue;

    // printf("load program header %d", i);

    uint32_t offset = phdr.p_offset;
    uint32_t file_size = phdr.p_filesz;
    uint32_t p_vaddr = phdr.p_vaddr;
    uint32_t mem_size = phdr.p_memsz;

    printf("load program from [%p, %p] to [%p, %p]\n", offset, file_size, p_vaddr, mem_size);
#ifdef USR_SPACE_ENABLE
    int left_size = file_size;
    fs_lseek(fd, offset, SEEK_SET);
    // printf("vaddr is %p\n", p_vaddr);
    if (!ISALIGN(p_vaddr))
    {
      void *pg_p = new_page(1);
      int read_len = min(PGSIZE - OFFSET(p_vaddr), left_size);
      left_size -= read_len;
      assert(fs_read(fd, pg_p + OFFSET(p_vaddr), read_len) >= 0);
      map(&pcb->as, (void *)p_vaddr, pg_p, PTE_R | PTE_W | PTE_X);
      p_vaddr += read_len;
    }

    for (; p_vaddr < phdr.p_vaddr + file_size; p_vaddr += PGSIZE)
    {
      assert(ISALIGN(p_vaddr));
      void *pg_p = new_page(1);
      memset(pg_p, 0, PGSIZE);
      // int len = min(PGSIZE, file_size - fs_lseek(fd, 0, SEEK_CUR));
      int read_len = min(PGSIZE, left_size);
      left_size -= read_len;
      assert(fs_read(fd, pg_p, read_len) >= 0);
      map(&pcb->as, (void *)p_vaddr, pg_p, PTE_R | PTE_W | PTE_X);
    }
    // printf("p_vaddr is %p\n", (void *)p_vaddr);
    p_vaddr = NEXT_PAGE(p_vaddr);
    printf("p_vaddr is %p next page, end of uninitialized space is %p\n", (void *)p_vaddr, (void *)(phdr.p_vaddr + mem_size));
    for (; p_vaddr < phdr.p_vaddr + mem_size; p_vaddr += PGSIZE)
    {
      assert(ISALIGN(p_vaddr));
      void *pg_p = new_page(1);
      memset(pg_p, 0, PGSIZE);
      map(&pcb->as, (void *)p_vaddr, pg_p, PTE_R | PTE_W | PTE_X);
    }
#else
    // ramdisk_read((void *)vaddr, offset, file_size);
    fs_lseek(fd, offset, SEEK_SET);
    fs_read(fd, (void *)p_vaddr, file_size);
    memset((void *)(p_vaddr + file_size), 0, mem_size - file_size);
#endif
    assert(mem_size >= file_size);
  }

  // printf("max brk is at %p when load\n", pcb->max_brk);
  assert(fs_close(fd) != -1);

  return entry;
}

void naive_uload(PCB *pcb, const char *filename)
{
  uintptr_t entry = loader(pcb, filename);
  Log("Jump to entry = %p", entry);
  ((void (*)())entry)();
}

Context *context_kload(PCB *pcb, void (*entry)(void *), void *arg)
{
  Area kstack = RANGE(pcb, (char *)pcb + STACK_SIZE);
  Context *ctx = kcontext(kstack, entry, arg);
  ctx->pdir = NULL;
  NR_PROC++;
  return ctx;
}

static int __len(char *const str[])
{
  int i = 0;
  // printf("str1 is NULL? %d\n", str[1] == NULL);
  for (; str[i] != NULL; ++i)
  {
    // printf("str is %d\n", i);
  }
  return i;
}

Context *context_uload(PCB *pcb, char *filename, char *const argv[], char *const envp[])
{
  protect(&pcb->as);
  // printf("in uload\n");
  // Important: must load after dealing with argv and envp!!!
  // they may be the data of the previous program (called by execve syscall, rather than init_proc)
  // found this bug when print printf("addr of envp is at %p\n", envp), it is inside previous program!!!
  //  uintptr_t entry = loader(pcb, filename);
  // printf("addr of envp is at %p\n", envp);
  Area kstack = RANGE(pcb, (char *)pcb + STACK_SIZE);
  // Context *ctx = ucontext(NULL, kstack, (void *)entry);
  //   printf("str1 is NULL %d\n", envp[1] == NULL);

  int argc = (argv == NULL) ? 0 : __len(argv);
  // printf("len arg is %d\n", argc);

  char *(args[argc]);
  char *sp = (char *)new_page(USR_STACK_PG_NUM) + USR_STACK_PG_NUM * PGSIZE;
  // memset(sp - USR_STACK_PG_NUM * PGSIZE, 0, USR_STACK_PG_NUM * PGSIZE);
#ifdef USR_SPACE_ENABLE
  for (size_t i = USR_STACK_PG_NUM; i > 0; --i)
    map(&pcb->as, (void *)(pcb->as.area.end - i * PGSIZE), sp - i * PGSIZE, PTE_R | PTE_W | PTE_X);
  uint32_t map_offset = sp - (char *)(pcb->as.area.end);
#endif
  // printf("map offset is %d\n", map_offset);
  for (int i = 0; i < argc; ++i)
  {
    sp -= (strlen(argv[i]) + 1);
    strcpy(sp, argv[i]);
    args[i] = sp;
  }

  int envc = (envp == NULL) ? 0 : __len(envp);

  char *(envs[envc]);
  for (int i = 0; i < envc; ++i)
  {
    sp -= (strlen(envp[i]) + 1);
    strcpy(sp, envp[i]);
    envs[i] = sp;
  }

  char **sp_2 = (char **)sp;
  --sp_2;
  *sp_2 = NULL;

  for (int i = envc - 1; i >= 0; --i)
  {
    // printf("loop %d\n", i);
    --sp_2;
    *sp_2 = envs[i];
  }

  --sp_2;
  *sp_2 = NULL;

  for (int i = argc - 1; i >= 0; --i)
  {
    --sp_2;
    // printf("the pos of argv is at stack %p is %p\n", sp_2, args[i]);
    *sp_2 = args[i];
    // printf("args is %s\n", args[i]);
  }

  --sp_2;
  *((int *)sp_2) = argc;

  uintptr_t entry = loader(pcb, filename);
  Context *ctx = ucontext(NULL, kstack, (void *)entry);
  ctx->pdir = pcb->as.ptr;

#ifdef USR_SPACE_ENABLE
  // ctx->GPRx = (uintptr_t)((char *)sp_2 - map_offset);
  // ctx->mscratch = ctx->GPRx;
  ctx->mscratch = (uintptr_t)((char *)sp_2 - map_offset);
// #else
// ctx->GPRx = (uintptr_t)((char *)sp_2);
#endif
  // printf("map sp %p to %p\n", sp_2, ctx->GPRx);
  pcb->max_brk = 0;

  // printf("the argc is at %p\n", sp_2);
  // printf("arg1 is %s\n", *((char **)(sp_2 + 2)));
  // printf("arg0 is %s\n", *((char **)(ctx->GPRx + 1)));
  NR_PROC++;

  return ctx;
}
