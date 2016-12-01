#ifndef _SYSDEF_H
#define _SYSDEF_H

#define SYSCALL_DATA_SIZE 512
#define SYSCALL_HEADER_SIZE 64
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
  char pad[64];
}SYSCALL_FILEIO;

typedef struct
{
  char data[512];
  char metadata[64];
}SYSCALL_STD;

typedef struct
{
  int size;
  int fd;
  char pad[64 - (sizeof(int))*2];
}SYSCALL_STREAM_METADATA;

typedef struct
{
  char data[SYSCALL_DATA_SIZE];
  SYSCALL_STREAM_METADATA metadata;
}SYSCALL_STREAM;

#endif
