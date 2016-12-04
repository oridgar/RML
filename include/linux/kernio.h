#ifndef _KERNIO_H
#define _KERNIO_H
#include <types.h>

typedef struct {
	char pathname[32];
	unsigned long pos;
	unsigned long size;
	int pfd;
	int pfd_offset;
} FileDescriptor;

typedef struct {
	char name[9];
	char ext[4];
	long size;
} linux_dirent;

int lba_to_chs(unsigned int offset, unsigned char cylinders, unsigned char heads, unsigned char sectors, unsigned char *cylinder, unsigned char *head, unsigned char *sector);
//int open(const char *pathname, int flags);
int open(const char *pathname, int flags,int pid);
int close(int fd,int pid);
off_t lseek(int fd, off_t offset, int whence,int pid);
unsigned int read(int fd, void *buf, unsigned int count,int pid);
int write(int fd, void *buf, unsigned int count,int pid);
void printk(char *string);
char *uitoa(unsigned int i);
int mount(char * filename);
void load_to_memory(unsigned int segment,int prog_offset,char *buffer,int count);
int getdents(unsigned int fd, linux_dirent *dirp, unsigned int count);

extern FileDescriptor fd_list[5];

#endif
