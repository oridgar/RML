#ifndef _SYSDEF_H
#define _SYSDEF_H

#define SYSCALL_BUF_SIZE 576

typedef struct 
{
  char param[SYSCALL_BUF_SIZE];
}SYSCALL_PARAM ;

typedef struct
{
  int fd;
  unsigned int count;
  char buf[508];
}SYSCALL_FILEIO;

#endif
