#ifndef _SCHED_H
#define _SCHED_H

#define NUM_PROCESSES 9
typedef enum {
	READY,
	WAIT,
	RUNNING
} procState;

typedef struct {
	unsigned int pid;
	char name[32];
	unsigned int  ss;
	unsigned int  ds;
	unsigned int  cs;
	unsigned int  ip;
	procState state;
} ProcessDesc;

void schedule();
void init_processes();
int register_proc(char *name, int cs, int ds, int ss,int entry_point,int *pid);
int unregister_proc(int pid);

#endif
