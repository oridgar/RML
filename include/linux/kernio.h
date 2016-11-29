#ifndef _KERNIO_H
#define _KERNIO_H
#include "types.h"

typedef struct {
	char pathname[32];
	int pos;
	unsigned int size;
	int pfd;
	int pfd_offset;
} FileDescriptor;

typedef struct {
	char name[9];
	char ext[4];
	long size;
} linux_dirent;

int lba_to_chs(off_t offset, char cylinders, char heads, char sectors, char *cylinder, char *head, char *sector);
int open(const char *pathname, int flags);
int close(int fd);
off_t lseek(int fd, off_t offset, int whence);
unsigned int read(int fd, void *buf, unsigned int count);
void printk(char *string);
char *myitoa(unsigned int i);
int mount(char * filename);
void load_to_memory(unsigned int segment,int prog_offset,char *buffer,int count);
int getdents(unsigned int fd, linux_dirent *dirp, unsigned int count);

#endif
