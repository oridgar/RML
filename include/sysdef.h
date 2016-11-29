#ifndef _SYSDEF_H
#define _SYSDEF_H

typedef struct 
{
  char param[512];
}SYSCALL_PARAM ;

typedef struct
{
  int fd;
  unsigned int count;
  char buf[508];
}SYSCALL_FILEIO;

#endif
