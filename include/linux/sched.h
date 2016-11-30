#ifndef _SCHED_H
#define _SCHED_H
#include <linux/kernio.h>
#define NUM_PROCESSES 9
#define NUM_FILES 5

typedef enum {
	READY,
	WAIT,
	RUNNING,
	SLEEP
} procState;

typedef struct {
	unsigned int pid;
	char name[32];
	unsigned int  ss;
	unsigned int  ds;
	unsigned int  cs;
	unsigned int  ip;
	unsigned int ppid;
	unsigned long size;
	procState state;
	FileDescriptor files_table[NUM_FILES];

} ProcessDesc;

extern ProcessDesc processes[NUM_PROCESSES];
void schedule();
void init_processes();
register_proc(char *name, int cs, int ds, int ss,int entry_point,int *pid,int ppid,unsigned long size);
int unregister_proc(int pid);

#endif
