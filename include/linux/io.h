//#include <sys/types.h>
//#include <unistd.h>
#ifndef _IO_H
#define _IO_H

#include "types.h"

void call_int();
void halt();
void syscall(int service);
//returns file descriptor
//int open(const char *pathname,int flags);
int myopen(const char *pathname, int flags, unsigned int mode);
//int open(const char *pathname, int flags, mode_t mode);
//int creat(const char *pathname, mode_t mode);
int mycreat(const char *pathname, unsigned int mode);
int myclose(int fd);
//ssize_t read(int fd, void *buf, size_t count);
//ssize_t write(int fd, const void *buf, size_t count);
//off_t lseek(int fildes, off_t offset, int whence);
unsigned int myread(int fd, void *buf, unsigned int count);
unsigned int mywrite(int fd, const void *buf, unsigned int count);
unsigned int mylseek(int fildes, unsigned int offset, int whence);

#endif
