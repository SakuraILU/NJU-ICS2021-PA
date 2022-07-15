#include <fs.h>
#include <klib.h>

#define NR_FILE (sizeof(file_table) / sizeof(file_table[0]))
#define min(x, y) ((x < y) ? x : y)

typedef size_t (*ReadFn)(void *buf, size_t offset, size_t len);
typedef size_t (*WriteFn)(const void *buf, size_t offset, size_t len);

typedef struct
{
  char *name;
  size_t size;
  size_t disk_offset;
  size_t open_offset;
  ReadFn read;
  WriteFn write;
} Finfo;

enum
{
  FD_STDIN,
  FD_STDOUT,
  FD_STDERR,
  FD_FB
};

static size_t invalid_read(void *buf, size_t offset, size_t len)
{
  panic("should not reach here");
  return 0;
}

static size_t invalid_write(const void *buf, size_t offset, size_t len)
{
  panic("should not reach here");
  return 0;
}

static size_t file_read(void *buf, size_t offset, size_t len)
{
  return ramdisk_read(buf, offset, len);
}

static size_t file_write(const void *buf, size_t offset, size_t len)
{
  return ramdisk_write(buf, offset, len);
}

/* This is the information about all files in disk. */
static Finfo file_table[] __attribute__((used)) = {
    [FD_STDIN] = {"stdin", 0, 0, 0, invalid_read, invalid_write},
    [FD_STDOUT] = {"stdout", 0, 0, 0, invalid_read, serial_write},
    [FD_STDERR] = {"stderr", 0, 0, 0, invalid_read, serial_write},
    {"/dev/events", 0, 0, 0, events_read, invalid_write},
    {"/proc/dispinfo", 50, 0, 0, dispinfo_read, invalid_write},
    {"/dev/fb", 0, 0, 0, invalid_read, fb_write},
#include "files.h"
};

void init_fs()
{
  for (size_t fd = 6; fd < NR_FILE; ++fd)
  {
    if (file_table[fd].write == NULL)
      file_table[fd].write = file_write;
    if (file_table[fd].read == NULL)
      file_table[fd].read = file_read;
  }
  // TODO: initialize the size of /dev/fb

  AM_GPU_CONFIG_T gpu_config;
  ioe_read(AM_GPU_CONFIG, &gpu_config);
  int width = gpu_config.width, height = gpu_config.height;
  int fb_fd = fs_open("/dev/fb", 0, 0);
  file_table[fb_fd].size = width * height;
}

int fs_open(const char *pathname, int flags, int mode)
{
  // printf("===============");
  for (size_t fd = 0; fd < NR_FILE; ++fd)
  {
    // printf("compare is %s with %s\n", file_table[fd].name, pathname);
    if (strcmp(file_table[fd].name, pathname) == 0)
    {
      file_table[fd].open_offset = 0;
      return fd;
    }
  }
  return -1;
}

size_t fs_write(int fd, const void *buf, size_t len)
{
  // printf("write fd is %d, size is %d, to write len is %d\n", fd, file_table[fd].size, len);
  if (fd == 0)
    panic("should never write to stdin");
  // assert(fd >= 0 && fd < NR_FILE);
  if (fd > 0 && fd < 5)
    return file_table[fd].write(buf, 0, len);
  if (fd >= 5 && fd < NR_FILE)
  {
    len = min(len, file_table[fd].size - file_table[fd].open_offset);
    // if (len == 0)
    //   return 0;
    size_t count = file_table[fd].write(buf, file_table[fd].disk_offset + file_table[fd].open_offset, len);
    // printf("count is %d, len is %d\n", count, len);
    file_table[fd].open_offset += count;
    // printf("open_offset is %d, size is %d, res is %d\n", file_table[fd].open_offset, file_table[fd].size, file_table[fd].open_offset < file_table[fd].size);
    // printf("open offset is less than size\n");
    assert((file_table[fd].open_offset <= file_table[fd].size));
    // printf("count is %d\n", count);
    return count;
  }
  else
    return -1;
}

size_t fs_read(int fd, void *buf, size_t len)
{
  // printf("read fd is %d, size is %d, offset is %d, to read len is %d\n", fd, file_table[fd].size, file_table[fd].open_offset, len);
  assert(fd > 0 && fd < NR_FILE);
  if (fd > 0 && fd < 5)
    return file_table[fd].read(buf, 0, len);
  if (fd >= 5 && fd < NR_FILE)
  {
    len = min(len, file_table[fd].size - file_table[fd].open_offset);
    // if (len == 0)
    //   return 0;
    // printf("read some thing from %d with size %d\n", file_table[fd].disk_offset + file_table[fd].open_offset, len);
    size_t count = file_table[fd].read(buf, file_table[fd].disk_offset + file_table[fd].open_offset, len);
    // printf("len is %d\n", count);
    // printf("read open_offset is %d, size is %d, res is %d\n", file_table[fd].open_offset, file_table[fd].size, file_table[fd].open_offset < file_table[fd].size);
    file_table[fd].open_offset += count;
    // printf("read open_offset is %d, size is %d, res is %d\n", file_table[fd].open_offset, file_table[fd].size, file_table[fd].open_offset < file_table[fd].size);
    assert((file_table[fd].open_offset <= file_table[fd].size));
    // printf("count is %d\n", count);
    return count;
  }
  else
    return -1;
}

size_t fs_lseek(int fd, size_t offset, int whence)
{
  // printf("change seek of fd %d\n", fd);
  assert(fd > 3 && fd < NR_FILE);
  switch (whence)
  {
  case SEEK_SET:
    file_table[fd].open_offset = offset;
    break;
  case SEEK_CUR:
    file_table[fd].open_offset += offset;
    break;
  case SEEK_END:
    file_table[fd].open_offset = file_table[fd].size + offset;
    break;
  default:
    return -1;
  }
  // printf("offset is %d\n", file_table[fd].open_offset);
  return file_table[fd].open_offset;
}

int fs_close(int fd)
{
  if (fd >= 3 && fd < NR_FILE)
  {
    file_table[fd].open_offset = 0;
    return 0;
  }
  else
    return -1;
}