#ifndef _KERNIO_H
#define _KERNIO_H
#include "types.h"

typedef struct {
	char pathname[32];
	int pos;
} FileDescriptor;

int lba_to_chs(off_t offset, char cylinders, char heads, char sectors, char *cylinder, char *head, char *sector);
int open(const char *pathname, int flags);
int close(int fd);
off_t lseek(int fd, off_t offset, int whence);
unsigned int read(int fd, void *buf, unsigned int count);
void printk(char *string);
char *myitoa(unsigned int i);
int mount(char * filename);
void load_to_memory(unsigned int segment,int prog_offset,char *buffer,int count);

#endif
