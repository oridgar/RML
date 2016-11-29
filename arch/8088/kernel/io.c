#include "io.h"
#include "sysdef.h"
#include "mystdio.h"
#include "mystring.h"

void syscall(int service);
void _putchar(char in);


SYSCALL_PARAM sysparam;

//TEMP REGION - DIRECT CALLING SCREEN AND KEYBOARD FROM BIOS
void dprintstr(char *string);
void dputchar(char in);
char dgetchar();


void dprintstr(char *string) {
	int i = 0;
	
	memcpy(sysparam.param,string,512);
	
	while (sysparam.param[i] != 0) {
		//dputchar(sysparam.param[i]);
		_putchar(sysparam.param[i]);
		i++;
	}
}

void dputchar(char in) {
	char temp = in;
	asm {
		  mov ah,0Eh
		  mov al,[temp]
		  int 10h
	}
}


char dgetchar() {
	char temp;
	
	waitForKey:
	asm {
		mov ah,01h
		int 16h
		jnz gotKey
		jmp waitForKey
	}

	gotKey:
	asm {
		mov ah,00h
		int 16h
		mov [temp],al
	}
	
	return temp;
}

//END TEMP REGION - DIRECT CALLING SCREEN AND KEYBOARD FROM BIOS


void syscall(int service) {
	asm {
		//not needed as code and stack are taken care
		//push cs
		//push ip
		//push ss
		//push sp
		

		//push bp already happen by c compiler		
		pushf
		push ds
		push es
		push ax
		push cx
		push di
		
		push bx
		push dx
		push si
		
		mov bx,[service] //copying the value sitting in "service" address (pass by value)
		mov dx,bx //copying the content of bx
		mov si,offset sysparam;
		int 80h
		pop si
		pop dx
		pop bx
		
		pop di
		pop cx
		pop ax
		pop es
		pop ds
		popf
	}
}

void call_int() {
	syscall(0x00);
}

void halt() {
	syscall(0x01);
}

void reboot() {
	syscall(0x02);
}

void _printstr(char *string) {
	memcpy(sysparam.param,string,512);
	syscall(0x04);
	//dprintstr(string);
}

void _putchar(char in) {
	sysparam.param[0] = in;
	syscall(0x05);
	//dputchar(in);
}

void dispatch() {
	syscall(0x05);
}

char _getchar() {
	syscall(0x06);
	return sysparam.param[0];
	//return dgetchar();
}

int run_program(char *string) {
	memcpy(sysparam.param,string,512);
	syscall(0x07);
	return sysparam.param[0];
}

void get_process_list() {
	syscall(0x08);
}


//files

/*
int myopen(const char *pathname, int flags, unsigned int mode) {
	memcpy(pathname,sysparam.param,256);
	syscall(0x08);
	return (int)sysparam.param;
}

int myclose(int fd) {
	memcpy(&fd,sysparam.param,sizeof(fd));
	syscall(0x09);
	return (int)sysparam.param;
}

unsigned int myread(int fd, void *buf, unsigned int count) {
	memcpy(fd,(&((SYSCALL_FILEIO)sysparam).fd),sizeof(fd));
	memcpy(&count,&(((SYSCALL_FILEIO)sysparam).count),sizeof(count));
	syscall(0x10);
	memcpy(&sysparam,buf,512);
	return 0;
}

unsigned int mywrite(int fd, const void *buf, unsigned int count) {
	SYSCALL_FILEIO *params = (SYSCALL_FILEIO)sysparam;
	memcpy(fd,&params->fd,sizeof(fd));
	memcpy(count,&params->count,sizeof(count));
	memcpy(buf,&params->buf,sizeof(params->buf));
	syscall(0x11);
	return sysparam;
}

unsigned int mylseek(int fildes, unsigned int offset, int whence) {
	return 0;
}
*/

/*
fork();
pid = fork() Create a child process identical to the parent
pid = waitpid(pid, &statloc, opts) Wait for a child to terminate
s = wait(&status) Old version of waitpid
s = execve(name, argv, envp) Replace a process core image
exit(status) Terminate process execution and return status
size = brk(addr) Set the size of the data segment
pid = getpid() Return the caller�s process id
pid = getpgrp() Return the id of the caller�s process group
pid = setsid() Create a new session and return its proc. group id
l = ptrace(req, pid, addr, data) Used for debugging
s = sigaction(sig, &act, &oldact) Define action to take on signals
s = sigreturn(&context) Return from a signal
s = sigprocmask(how, &set, &old) Examine or change the signal mask
s = sigpending(set) Get the set of blocked signals
s = sigsuspend(sigmask) Replace the signal mask and suspend the process
s = kill(pid, sig) Send a signal to a process
residual = alarm(seconds) Set the alarm clock signal
s = pause() Suspend the caller until the next signal
fd = creat(name, mode) Obsolete way to create a new file
fd = mknod(name, mode, addr) Create a regular, special, or directory i-node
fd = open(file, how, ...) Open a file for reading, writing or both
s = close(fd) Close an open file
n = read(fd, buffer, nbytes) Read data from a file into a buffer
n = write(fd, buffer, nbytes) Write data from a buffer into a file
pos = lseek(fd, offset, whence) Move the file pointer
s = stat(name, &buf) Get a file�s status information
s = fstat(fd, &buf) Get a file�s status information
fd = dup(fd) Allocate a new file descriptor for an open file
s = pipe(&fd[0]) Create a pipe
s = ioctl(fd, request, argp) Perform special operations on a file
s = access(name, amode) Check a file�s accessibility
s = rename(old, new) Give a file a new name
s = fcntl(fd, cmd, ...) File locking and other operations
s = mkdir(name, mode) Create a new directory
s = rmdir(name) Remove an empty directory
s = link(name1, name2) Create a new entry, name2, pointing to name1
s = unlink(name) Remove a directory entry
s = mount(special, name, flag) Mount a file system
Call Description
s = umount(special) Unmount a file system
s = sync() Flush all cached blocks to the disk
s = chdir(dirname) Change the working directory
s = chroot(dirname) Change the root directory
s = chmod(name, mode) Change a file�s protection bits
uid = getuid() Get the caller�s uid 
gid = getgid() Get the caller�s gid
s = setuid(uid) Set the caller�s uid
s = setgid(gid) Set the caller�s gid
s = chown(name, owner, group) Change a file�s owner and group
oldmask = umask(complmode) Change the mode mask
seconds = time(&seconds) Get the elapsed time since Jan. 1, 1970
s = stime(tp) Set the elapsed time since Jan. 1, 1970
s = utime(file, timep) Set a file�s "last access"time
s = times(buffer) Get the user and system times used so far )
*/



/*
       System call                Kernel        Notes
       --------------------------------------------------------------------

       _llseek(2)                 1.2
       _newselect(2)              2.0
       _sysctl(2)                 2.0
       accept(2)                  2.0           See notes on socketcall(2)
       accept4(2)                 2.6.28
       access(2)                  1.0
       acct(2)                    1.0
       add_key(2)                 2.6.11
       adjtimex(2)                1.0
       alarm(2)                   1.0
       alloc_hugepages(2)         2.5.36        Removed in 2.5.44
       bdflush(2)                 1.2           Deprecated (does nothing)
                                                since 2.6
       bind(2)                    2.0           See notes on socketcall(2)
       bpf(2)                     3.18
       brk(2)                     1.0
       cacheflush(2)              1.2           Not on x86
       capget(2)                  2.2
       capset(2)                  2.2
       chdir(2)                   1.0
       chmod(2)                   1.0
       chown(2)                   2.2           See chown(2) for
                                                version details
       chown32(2)                 2.4
       chroot(2)                  1.0
       clock_adjtime(2)           2.6.39
       clock_getres(2)            2.6
       clock_gettime(2)           2.6
       clock_nanosleep(2)         2.6
       clock_settime(2)           2.6

       clone(2)                   1.0
       close(2)                   1.0
       connect(2)                 2.0           See notes on socketcall(2)
       copy_file_range(2)         4.5
       creat(2)                   1.0
       create_module(2)           1.0           Removed in 2.6
       delete_module(2)           1.0
       dup(2)                     1.0
       dup2(2)                    1.0
       dup3(2)                    2.6.27
       epoll_create(2)            2.6
       epoll_create1(2)           2.6.27
       epoll_ctl(2)               2.6
       epoll_pwait(2)             2.6.19
       epoll_wait(2)              2.6
       eventfd(2)                 2.6.22
       eventfd2(2)                2.6.27
       execve(2)                  1.0
       execveat(2)                3.19
       exit(2)                    1.0
       exit_group(2)              2.6
       faccessat(2)               2.6.16
       fadvise64(2)               2.6
       fadvise64_64(2)            2.6
       fallocate(2)               2.6.23
       fanotify_init(2)           2.6.37
       fanotify_mark(2)           2.6.37
       fchdir(2)                  1.0
       fchmod(2)                  1.0
       fchmodat(2)                2.6.16
       fchown(2)                  1.0
       fchown32(2)                2.4
       fchownat(2)                2.6.16
       fcntl(2)                   1.0
       fcntl64(2)                 2.4
       fdatasync(2)               2.0
       fgetxattr(2)               2.6; 2.4.18
       finit_module(2)            3.8
       flistxattr(2)              2.6; 2.4.18
       flock(2)                   2.0
       fork(2)                    1.0
       free_hugepages(2)          2.5.36        Removed in 2.5.44
       fremovexattr(2)            2.6; 2.4.18
       fsetxattr(2)               2.6; 2.4.18
       fstat(2)                   1.0
       fstat64(2)                 2.4
       fstatat64(2)               2.6.16
       fstatfs(2)                 1.0
       fstatfs64(2)               2.6
       fsync(2)                   1.0
       ftruncate(2)               1.0
       ftruncate64(2)             2.4
       futex(2)                   2.6
       futimesat(2)               2.6.16
       get_kernel_syms(2)         1.0           Removed in 2.6
       get_mempolicy(2)           2.6.6
       get_robust_list(2)         2.6.17
       get_thread_area(2)         2.6
       getcpu(2)                  2.6.19
       getcwd(2)                  2.2
       getdents(2)                2.0
       getdents64(2)              2.4
       getegid(2)                 1.0
       getegid32(2)               2.4
       geteuid(2)                 1.0

       geteuid32(2)               2.4
       getgid(2)                  1.0
       getgid32(2)                2.4
       getgroups(2)               1.0
       getgroups32(2)             2.4
       getitimer(2)               1.0
       getpeername(2)             2.0           See notes on socketcall(2)
       getpagesize(2)             2.0           Not on x86
       getpgid(2)                 1.0
       getpgrp(2)                 1.0
       getpid(2)                  1.0
       getppid(2)                 1.0
       getpriority(2)             1.0
       getrandom(2)               3.17
       getresgid(2)               2.2
       getresgid32(2)             2.4
       getresuid(2)               2.2
       getresuid32(2)             2.4
       getrlimit(2)               1.0
       getrusage(2)               1.0
       getsid(2)                  2.0
       getsockname(2)             2.0           See notes on socketcall(2)
       getsockopt(2)              2.0           See notes on socketcall(2)
       gettid(2)                  2.4.11
       gettimeofday(2)            1.0
       getuid(2)                  1.0
       getuid32(2)                2.4
       getxattr(2)                2.6; 2.4.18
       init_module(2)             1.0
       inotify_add_watch(2)       2.6.13
       inotify_init(2)            2.6.13
       inotify_init1(2)           2.6.27
       inotify_rm_watch(2)        2.6.13
       io_cancel(2)               2.6
       io_destroy(2)              2.6
       io_getevents(2)            2.6
       io_setup(2)                2.6
       io_submit(2)               2.6
       ioctl(2)                   1.0
       ioperm(2)                  1.0
       iopl(2)                    1.0
       ioprio_get(2)              2.6.13
       ioprio_set(2)              2.6.13
       ipc(2)                     1.0
       kcmp(2)                    3.5
       kern_features(2)           3.7           Sparc64
       kexec_file_load(2)         3.17
       kexec_load(2)              2.6.13
       keyctl(2)                  2.6.11
       kill(2)                    1.0
       lchown(2)                  1.0           See chown(2) for
                                                version details
       lchown32(2)                2.4
       lgetxattr(2)               2.6; 2.4.18
       link(2)                    1.0
       linkat(2)                  2.6.16
       listen(2)                  2.0           See notes on socketcall(2)
       listxattr(2)               2.6; 2.4.18
       llistxattr(2)              2.6; 2.4.18
       lookup_dcookie(2)          2.6
       lremovexattr(2)            2.6; 2.4.18
       lseek(2)                   1.0
       lsetxattr(2)               2.6; 2.4.18
       lstat(2)                   1.0
       lstat64(2)                 2.4

       madvise(2)                 2.4
       mbind(2)                   2.6.6
       memfd_create(2)            3.17
       migrate_pages(2)           2.6.16
       mincore(2)                 2.4
       mkdir(2)                   1.0
       mkdirat(2)                 2.6.16
       mknod(2)                   1.0
       mknodat(2)                 2.6.16
       mlock(2)                   2.0
       mlock2(2)                  4.4
       mlockall(2)                2.0
       mmap(2)                    1.0
       mmap2(2)                   2.4
       modify_ldt(2)              1.0
       mount(2)                   1.0
       move_pages(2)              2.6.18
       mprotect(2)                1.0
       mq_getsetattr(2)           2.6.6
       mq_notify(2)               2.6.6
       mq_open(2)                 2.6.6
       mq_timedreceive(2)         2.6.6
       mq_timedsend(2)            2.6.6
       mq_unlink(2)               2.6.6
       mremap(2)                  2.0
       msgctl(2)                  2.0           See notes on ipc(2)
       msgget(2)                  2.0           See notes on ipc(2)
       msgrcv(2)                  2.0           See notes on ipc(2)
       msgsnd(2)                  2.0           See notes on ipc(2)
       msync(2)                   2.0
       munlock(2)                 2.0
       munlockall(2)              2.0
       munmap(2)                  1.0
       name_to_handle_at(2)       2.6.39
       nanosleep(2)               2.0
       nfsservctl(2)              2.2           Removed in 3.1
       nice(2)                    1.0
       oldfstat(2)                1.0
       oldlstat(2)                1.0
       oldolduname(2)             1.0
       oldstat(2)                 1.0
       olduname(2)                1.0
       open(2)                    1.0
       open_by_handle_at(2)       2.6.39
       openat(2)                  2.6.16
       pause(2)                   1.0
       pciconfig_iobase(2)        2.2.15; 2.4   Not on x86
       pciconfig_read(2)          2.0.26; 2.2   Not on x86
       pciconfig_write(2)         2.0.26; 2.2   Not on x86
       perf_event_open(2)         2.6.31        Was perf_counter_open() in
                                                2.6.31; renamed in 2.6.32
       personality(2)             1.2
       perfctr(2)                 2.2           Sparc; removed in 2.6.34
       perfmonctl(2)              2.4           ia64
       pipe(2)                    1.0
       pipe2(2)                   2.6.27
       pivot_root(2)              2.4
       poll(2)                    2.0.36; 2.2
       ppc_rtas(2)                2.6.2         PowerPC only
       ppoll(2)                   2.6.16
       prctl(2)                   2.2
       pread64(2)                               Added as "pread" in 2.2;
                                                renamed "pread64" in 2.6
       preadv(2)                  2.6.30
       preadv2(2)                 4.6

       prlimit64(2)               2.6.36
       process_vm_readv(2)        3.2
       process_vm_writev(2)       3.2
       pselect6(2)                2.6.16
       ptrace(2)                  1.0
       pwrite64(2)                              Added as "pwrite" in 2.2;
                                                renamed "pwrite64" in 2.6
       pwritev(2)                 2.6.30
       pwritev2(2)                4.6
       query_module(2)            2.2           Removed in 2.6
       quotactl(2)                1.0
       read(2)                    1.0
       readahead(2)               2.4.13
       readdir(2)                 1.0
       readlink(2)                1.0
       readlinkat(2)              2.6.16
       readv(2)                   2.0
       reboot(2)                  1.0
       recv(2)                    2.0           See notes on socketcall(2)
       recvfrom(2)                2.0           See notes on socketcall(2)
       recvmsg(2)                 2.0           See notes on socketcall(2)
       recvmmsg(2)                2.6.33
       remap_file_pages(2)        2.6           Deprecated since 3.16
       removexattr(2)             2.6; 2.4.18
       rename(2)                  1.0
       renameat(2)                2.6.16
       renameat2(2)               3.15
       request_key(2)             2.6.11
       restart_syscall(2)         2.6
       rmdir(2)                   1.0
       rt_sigaction(2)            2.2
       rt_sigpending(2)           2.2
       rt_sigprocmask(2)          2.2
       rt_sigqueueinfo(2)         2.2
       rt_sigreturn(2)            2.2
       rt_sigsuspend(2)           2.2
       rt_sigtimedwait(2)         2.2
       rt_tgsigqueueinfo(2)       2.6.31
       s390_runtime_instr(2)      3.7           s390 only
       s390_pci_mmio_read(2)      3.19          s390 only
       s390_pci_mmio_write(2)     3.19          s390 only
       sched_get_priority_max(2)  2.0
       sched_get_priority_min(2)  2.0
       sched_getaffinity(2)       2.6
       sched_getattr(2)           3.14
       sched_getparam(2)          2.0
       sched_getscheduler(2)      2.0
       sched_rr_get_interval(2)   2.0
       sched_setaffinity(2)       2.6
       sched_setattr(2)           3.14
       sched_setparam(2)          2.0
       sched_setscheduler(2)      2.0
       sched_yield(2)             2.0
       seccomp(2)                 3.17
       select(2)                  1.0
       semctl(2)                  2.0           See notes on ipc(2)
       semget(2)                  2.0           See notes on ipc(2)
       semop(2)                   2.0           See notes on ipc(2)
       semtimedop(2)              2.6; 2.4.22
       send(2)                    2.0           See notes on socketcall(2)
       sendfile(2)                2.2
       sendfile64(2)              2.6; 2.4.19
       sendmmsg(2)                3.0
       sendmsg(2)                 2.0           See notes on socketcall(2)
       sendto(2)                  2.0           See notes on socketcall(2)

       set_mempolicy(2)           2.6.6
       set_robust_list(2)         2.6.17
       set_thread_area(2)         2.6
       set_tid_address(2)         2.6
       setdomainname(2)           1.0
       setfsgid(2)                1.2
       setfsgid32(2)              2.4
       setfsuid(2)                1.2
       setfsuid32(2)              2.4
       setgid(2)                  1.0
       setgid32(2)                2.4
       setgroups(2)               1.0
       setgroups32(2)             2.4
       sethostname(2)             1.0
       setitimer(2)               1.0
       setns(2)                   3.0
       setpgid(2)                 1.0
       setpriority(2)             1.0
       setregid(2)                1.0
       setregid32(2)              2.4
       setresgid(2)               2.2
       setresgid32(2)             2.4
       setresuid(2)               2.2
       setresuid32(2)             2.4
       setreuid(2)                1.0
       setreuid32(2)              2.4
       setrlimit(2)               1.0
       setsid(2)                  1.0
       setsockopt(2)              2.0           See notes on socketcall(2)
       settimeofday(2)            1.0
       setuid(2)                  1.0
       setuid32(2)                2.4
       setup(2)                   1.0           Removed in 2.2
       setxattr(2)                2.6; 2.4.18
       sgetmask(2)                1.0
       shmat(2)                   2.0           See notes on ipc(2)
       shmctl(2)                  2.0           See notes on ipc(2)
       shmdt(2)                   2.0           See notes on ipc(2)
       shmget(2)                  2.0           See notes on ipc(2)
       shutdown(2)                2.0           See notes on socketcall(2)
       sigaction(2)               1.0
       sigaltstack(2)             2.2
       signal(2)                  1.0
       signalfd(2)                2.6.22
       signalfd4(2)               2.6.27
       sigpending(2)              1.0
       sigprocmask(2)             1.0
       sigreturn(2)               1.0
       sigsuspend(2)              1.0
       socket(2)                  2.0           See notes on socketcall(2)
       socketcall(2)              1.0
       socketpair(2)              2.0           See notes on socketcall(2)
       splice(2)                  2.6.17
       spu_create(2)              2.6.16        PowerPC only
       spu_run(2)                 2.6.16        PowerPC only
       ssetmask(2)                1.0
       stat(2)                    1.0
       stat64(2)                  2.4
       statfs(2)                  1.0
       statfs64(2)                2.6
       stime(2)                   1.0
       subpage_prot(2)            2.6.25        PowerPC only
       swapoff(2)                 1.0
       swapon(2)                  1.0
       symlink(2)                 1.0

       symlinkat(2)               2.6.16
       sync(2)                    1.0
       sync_file_range(2)         2.6.17
       sync_file_range2(2)        2.6.22
       syncfs(2)                  2.6.39
       sysfs(2)                   1.2
       sysinfo(2)                 1.0
       syslog(2)                  1.0
       tee(2)                     2.6.17
       tgkill(2)                  2.6
       time(2)                    1.0
       timer_create(2)            2.6
       timer_delete(2)            2.6
       timer_getoverrun(2)        2.6
       timer_gettime(2)           2.6
       timer_settime(2)           2.6
       timerfd_create(2)          2.6.25
       timerfd_gettime(2)         2.6.25
       timerfd_settime(2)         2.6.25
       times(2)                   1.0
       tkill(2)                   2.6; 2.4.22
       truncate(2)                1.0
       truncate64(2)              2.4
       ugetrlimit(2)              2.4
       umask(2)                   1.0
       umount(2)                  1.0
       umount2(2)                 2.2
       uname(2)                   1.0
       unlink(2)                  1.0
       unlinkat(2)                2.6.16
       unshare(2)                 2.6.16
       uselib(2)                  1.0
       ustat(2)                   1.0
       userfaultfd(2)             4.2
       utime(2)                   1.0
       utimensat(2)               2.6.22
       utimes(2)                  2.2
       utrap_install(2)           2.2           Sparc only
       vfork(2)                   2.2
       vhangup(2)                 1.0
       vm86old(2)                 1.0           Was "vm86"; renamed in
                                                2.0.28/2.2
       vm86(2)                    2.0.28; 2.2
       vmsplice(2)                2.6.17
       wait4(2)                   1.0
       waitid(2)                  2.6.10
       waitpid(2)                 1.0
       write(2)                   1.0
       writev(2)                  2.0
*/
