#ifndef __FS_H__
#define __FS_H__

#include <common.h>

// 从ramdisk中`offset`偏移处的`len`字节读入到`buf`中
size_t ramdisk_read(void *buf, size_t offset, size_t len);

// 把`buf`中的`len`字节写入到ramdisk中`offset`偏移处
size_t ramdisk_write(const void *buf, size_t offset, size_t len);

// 返回ramdisk的大小, 单位为字节
size_t get_ramdisk_size();


size_t serial_write(const void *buf, size_t offset, size_t len);
size_t events_read(void *buf, size_t offset, size_t len);
size_t dispinfo_read(void *buf, size_t offset, size_t len);
size_t fb_write(const void *buf, size_t offset, size_t len);


int fs_open(const char *pathname, int flags, int mode);
size_t fs_write(int fd, const void *buf, size_t count);
size_t fs_read(int fd, void *buf, size_t len);
size_t fs_lseek(int fd, size_t offset, int whence);
int fs_close(int fd);


#ifndef SEEK_SET
enum {SEEK_SET, SEEK_CUR, SEEK_END};
#endif

#endif
