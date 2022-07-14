#include <proc.h>
#include <elf.h>
#include <klib.h>
#include <fs.h>

#ifdef __LP64__
# define Elf_Ehdr Elf64_Ehdr
# define Elf_Phdr Elf64_Phdr
#else
# define Elf_Ehdr Elf32_Ehdr
# define Elf_Phdr Elf32_Phdr
#endif


// 从ramdisk中`offset`偏移处的`len`字节读入到`buf`中
// size_t ramdisk_read(void *buf, size_t offset, size_t len);

// 把`buf`中的`len`字节写入到ramdisk中`offset`偏移处
// size_t ramdisk_write(const void *buf, size_t offset, size_t len);

// 返回ramdisk的大小, 单位为字节
// size_t get_ramdisk_size();


static uintptr_t loader(PCB *pcb, const char *filename) {
  Elf_Ehdr ehdr;
  
  // ramdisk_read(&ehdr, 0, sizeof(ehdr));
  int fd = fs_open(filename, 0, 0);
  assert(fd != -1);

  fs_read(fd, &ehdr, sizeof(ehdr));

  char riscv32_magic_num[] = {0x7f, 0x45, 0x4c, 0x46, 0x01, 0x01 ,0x01 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00};
  // printf("magic number is %s\n", (char *)(ehdr.e_ident));
  assert(strcmp((char *)(ehdr.e_ident), riscv32_magic_num)== 0);

  uint32_t entry = ehdr.e_entry;
  uint32_t ph_offset = ehdr.e_phoff;
  uint32_t ph_num = ehdr.e_phnum;

  Elf_Phdr phdr;
  for(int i = 0; i < ph_num; ++i){
    ramdisk_read(&phdr, ph_offset + i * sizeof(phdr), sizeof(phdr));
    fs_lseek(fd, ph_offset + i * sizeof(phdr), SEEK_SET);
    fs_read(fd, &phdr, sizeof(phdr));
    if(phdr.p_type != PT_LOAD)continue;

    // printf("load program header %d", i);
    
    uint32_t offset = phdr.p_offset;
    uint32_t file_size = phdr.p_filesz;
    uint32_t vaddr = phdr.p_vaddr;
    uint32_t mem_size = phdr.p_memsz;

    // printf("load program from [%p, %p] to [%p, %p]\n", offset, file_size, vaddr, mem_size);

    // ramdisk_read((void *)vaddr, offset, file_size);
    fs_lseek(fd, offset, SEEK_SET);
    fs_read(fd, (void *)vaddr, file_size);
    
    assert(mem_size >= file_size);
    memset((void*)(vaddr + file_size), 0, mem_size - file_size);
  }

  assert(fs_close(fd) != -1);
  
  return entry;
}

void naive_uload(PCB *pcb, const char *filename) {
  uintptr_t entry = loader(pcb, filename);
  Log("Jump to entry = %p", entry);
  ((void(*)())entry) ();
}

