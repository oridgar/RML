#ifndef _SYSDEF_H
#define _SYSDEF_H

#define SYSCALL_DATA_SIZE 512
#define SYSCALL_HEADER_SIZE 64
#define SYSCALL_BUF_SIZE (SYSCALL_DATA_SIZE + SYSCALL_HEADER_SIZE)


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
  unsigned int size;
  int fd;
  unsigned int offset;
  //char pad[64 - (sizeof(int))*2];
}SYSCALL_STREAM_METADATA;

typedef struct
{
  char data[SYSCALL_DATA_SIZE];
  SYSCALL_STREAM_METADATA metadata;
}SYSCALL_STREAM;

typedef struct
{
  char name[9];
  char ext[4];
  char dir;
  char read_only;
  unsigned long size;
  char day;
  char month;
  unsigned short year;
  char seconds;
  char minutes;
  char hours;
}SYSCALL_FILE_ENTRY;

typedef struct
{
	unsigned int files_read;
}SYSCALL_FILE_ENTRY_METADATA;

typedef enum {FILE_OUT_LIST,FILE_OUT_WIDE} file_out_format;

#endif
