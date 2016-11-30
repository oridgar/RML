#include <linux\kernio.h>
#include <linux\sched.h>

ProcessDesc processes[NUM_PROCESSES];
int running_proc;

void schedule() {
	//printk("\r\nrunning process: ");
	//printk(myitoa(get_running_proc()));
	return;
}

void init_processes() {
	int i;
	for(i=0; i < NUM_PROCESSES; i++) {
		processes[i].pid = 0;
	}
	running_proc = 0;
}

void set_running_proc(int pid) {
	//TODO: put the process data in the same index as pid (index == pid)
	if (running_proc != 0) {
		processes[running_proc -1].state = READY;
	}
	running_proc = pid;
	processes[pid-1].state = RUNNING;
}

int get_running_proc() {
	return running_proc;
}

int register_proc(char *name, int cs, int ds, int ss,int entry_point,int *pid,int ppid,unsigned long size) {
	int i;
	for(i=0; i < NUM_PROCESSES; i++) {
		if(processes[i].pid == 0) {
			processes[i].pid = i+1;
			processes[i].cs = cs;
			processes[i].ds = ds;
			processes[i].ss = ss;
			processes[i].state = READY;
			processes[i].ip = entry_point;
			processes[i].ppid = ppid;
			processes[i].size = size;
			strcpy(processes[i].name,name);
			//copy also the name
			*pid = processes[i].pid;
			return processes[i].pid;
		}
	}
	return -1;
}

int unregister_proc(int pid) {
	processes[pid-1].pid = 0;
	return 0;
}

int get_process_list() {
	int i;
	printk("pid ppid cs name status size\r\n");
	for(i=0; i < NUM_PROCESSES; i++) {
		if(processes[i].pid != 0) {
			printk(uitoa(processes[i].pid));
			printk(" ");
			printk(uitoa(processes[i].ppid));
			printk(" ");
			printk(uitoa(processes[i].cs));
			printk(" ");
			printk(processes[i].name);
			printk(" ");
			switch (processes[i].state) {
			case READY:
				printk("READY");
				break;
			case WAIT:
				printk("WAIT");
				break;
			case RUNNING:
				printk("RUNNING");
				break;
			default:
				printk("UNKNOWN");
			}
			printk(" ");
			printk(uitoa(processes[i].size));
			printk("\r\n");
		}
	}
	return 0;
}
