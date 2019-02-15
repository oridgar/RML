#include <sysdef.h>
#include <linux/mm.h>
#include <fs/fs.h>
#include <types.h>
#include <bios.h>
#include <linux/kernio.h>
#include <linux/sched.h>
#include <linux/kernel.h>
#include <ctype.h>
#include <string.h>
#include <utils.h>

extern void farcall(int seg,int ofs); //call the program
extern int b_int8_seg;
extern int b_int8_offs;
void time();


//-------------------------
//CPU INSTRUCTIONS WRAPPERS
//-------------------------

void halt() {
	__asm__ __volatile__ (
	  "  hlt\n"
	  :
	  :
	  :"memory", "esi", "edi", "eax", "ebx", "ecx", "edx"
	);
}

void cli() {
	__asm__ __volatile__ (
	  "  cli\n"
	  :
	  :
	  :"memory", "esi", "edi", "eax", "ebx", "ecx", "edx"
	);
}

void sti() {
	__asm__ __volatile__ (
	  "  sti\n"
	  :
	  :
	  :"memory", "esi", "edi", "eax", "ebx", "ecx", "edx"
	);
}

void outb(int port, char value) {
	__asm__ __volatile__ (

	  "  push %%ax\n"
	  "  push %%dx\n"
	  "  mov  %0, %%dx\n"
	  "  mov  %1, %%al\n"
	  "  out  %%al, %%dx\n"
	  "  pop  %%dx\n"
	  "  pop  %%ax\n"

	  :
	  :"m"(port), "m"(value)
	  :"memory", "esi", "edi", "eax", "ebx", "ecx", "edx"
	);
}

//-----------------------------
//END CPU INSTRUCTIONS WRAPPERS
//-----------------------------

void startk() {
	//--------------------
	//Variable Declaration
	//--------------------
	int i;


    //------------
    //Code Section
    //------------
	open("/dev/stdout", 0, 0);
	printk("starting kernel...\r\n");
//	printk("buffer size: ");
//	printk(uitoa(SYSCALL_BUF_SIZE));
//	printk("\r\n");
	init_seg();
	set_ivt();
//	set_scheduler();
	//sti();

	mount("/dev/fdb");
	open("/dev/stdin", 0,0);
	sti();

	while (1) {
		init_processes();
		run_program("init");
	}
}

void dispatch(int service,SYSCALL_PARAM *params) {
	int curr_pid;
	int fd;
	//temp
	int files_read;
	int i;

	curr_pid = get_running_proc();
	//printk("We're in dispatch\r\n");
	switch (service) {
		case 0:
			write(1,"system call!\r\n",14,0);
			break;
		case 1:
			halt();
			break;
		case 2:
			reboot();
			break;
		case 3:
			break;
		case 4:
			write(1,params->param,strlen(params->param),0);
			break;
		case 5:
			write(1,params->param,1,0);
			break;
		case 6:
			read(0,params->param,1,0);
			break;
		case 7:
			*(int *)params->param = run_program(params->param);
			set_running_proc(curr_pid);
			break;
		case 8:
			get_process_list();
			break;
		case 9:
			fd = open("/",0,0);
			lseek(fd,0,0,0);
			files_read = list_root_files(512,((SYSCALL_STD*)params)->data ,FILE_OUT_LIST);
			((SYSCALL_FILE_ENTRY_METADATA *)((SYSCALL_STD*)params)->metadata)->files_read = files_read;
			break;
		case 10:
			fd = open(((SYSCALL_STREAM*)params)->data, 0,curr_pid);
			((SYSCALL_STREAM_METADATA *)((SYSCALL_STD*)params)->metadata)->fd = fd;
			break;
		case 11:
			close(((SYSCALL_STREAM*)params)->metadata.fd,curr_pid);
			break;
		case 12:
			lseek(((SYSCALL_STREAM*)params)->metadata.fd, ((SYSCALL_STREAM*)params)->metadata.offset, 0,curr_pid);
			break;
		case 13:
			read(((SYSCALL_STREAM*)params)->metadata.fd, ((SYSCALL_STREAM*)params)->data, ((SYSCALL_STREAM*)params)->metadata.size,curr_pid);
			break;
		case 14:
			write(((SYSCALL_STREAM*)params)->metadata.fd, ((SYSCALL_STREAM*)params)->data, ((SYSCALL_STREAM*)params)->metadata.size,curr_pid);
			break;
		case 15:
			time();
			break;
		default:
			break;
	}
	return;
}

unsigned int load(char *name,int *pid,unsigned int *seg) {
	unsigned int segment = get_free_seg();
	int offset = 0x0000;
	char num_sectors;
	char cylinder;
	char sector;
	char head;
	char drive;
	Fat16Entry file_list[256];
	Fat16Entry currfile;
	int retcode = 0;
	
	get_root_files(file_list);
	stoupper(name);
	if (get_file(file_list,name,&currfile) == -1) {
		//printk("could not find file!!!\r\n");
		retcode = 1;
	}
	else {
		//printk("calling load_file_content\r\n");
		//printk("starting cluster: ");
		//printk(uitoa(currfile.starting_cluster));
		//printk("\r\n");
		load_file_content(&currfile,segment,offset);
	}

	if (retcode == 0) {
		register_proc(name,segment,segment,segment,offset,pid,get_running_proc(),currfile.file_size);
	}
	
	*seg = segment;
	//return segment;
	return retcode;
}

void load_program(unsigned int segment,int prog_offset, char num_sectors,char cylinder,char sector, char head, char drive) {
	__asm__ __volatile__ (
	  "  mov  %0, %%dl\n"   //drive
	  "  mov  %1, %%ch\n"   // cylinder/track
	  "  mov  %2, %%dh\n"   //head 0
	  "  mov  %3, %%cl\n"
	  "  mov  %4, %%al\n"   //number of sectors to read
	  "  mov  %5, %%bx\n"
	  "  mov  %%bx, %%es\n"   //second 64K segment (pass by value)
	  "  mov  %6, %%bx\n"   //load to prog_offset from segment
	  "  mov  $0x2, %%ah\n" //Read Sectors From Drive service
	  "  int  $0x13\n"
	  :
	  :"m"(drive), "m"(cylinder), "m"(head), "m"(sector), "m"(num_sectors), "m"(segment), "m"(prog_offset)
	  :"memory", "esi", "edi", "eax", "ebx", "ecx", "edx"
	);
}

void uname() {
	return;
}

void set_ivt() {
	int segment = 0x1000;
	int offset = 0x0;

	__asm__ __volatile__ (
	  //first segment, where IVT is found
	  "  mov  $0x0, %%ax\n"
	  "  mov  %%ax, %%es\n"

	  //offset where ISR is found (0h)
	  //each entry in the IVT is 4 bytes. two bytes for offset and 2 bytes for segment
	  //80h * 04h = 200h
	  //at the next two bytes (202h) resides the segment for the interupt handler
	  "  mov  $0x200, %%bx\n"
	  "  mov  %0, %%ax\n"
	  "  movw %%ax, %%es:(%%bx)\n"

	  //segment where ISR is found (1000h - second segment)
	  "  mov  $0x202, %%bx\n"
	  "  mov  %1, %%ax\n"
	  "  movw %%ax, %%es:(%%bx)\n"
	  :
	  :"m"(offset), "m"(segment)
	  :"memory", "esi", "edi", "eax", "ebx", "ecx", "edx"
	);
	/*
	ivt:		; first segment set at ES
			mov ax,0h
			mov es,ax ; ivt is in first segment
						
			; position for INT 80h in IVT - offset of ISR
			mov bx,200h ; int 80h pos
			;mov ax,100h ; address of ISR
			mov ax,0h ; address of ISR
			mov es:[bx],ax ; location of the code for interrupt
			
			; position for INT 80h in IVT - segment of ISR
			mov bx,202h
			mov ax,1000h
			mov es:[bx],ax ; segment for interrupt 80h
	*/
}

//running a program by name
//calling load to load the program into memory
//calling farcall to run the program
int run_program(char *name) {
	unsigned int segment;
	//char seg_prefix;
	int pid;
	int retcode;

//	printk("loading ");
//	printk(name);
//	printk("...\r\n");

	//segment = load(name,&pid);
	retcode = load(name,&pid,&segment);

	if (retcode == 0) {
		//printstr("loaded to 0x");
		//seg_prefix = '0' + (segment / 0x1000);
		//_putchar(seg_prefix);
	//	printstr("000");
	//	printk(" pid: ");
	//	printk(myitoa(pid));
	//	printk(" ppid: ");
	//	printk(myitoa(get_running_proc()));
	//	printk("\r\n");


		//For now supporting only binary format. code origin is in 0x0000 or 0h
		//printk("set_running_proc\r\n");
		set_running_proc(pid);
		open("/dev/stdin",0,pid);
		open("/dev/stdout",0,pid);
		farcall(segment,0x0000);
	
	//	printstr("returned to kernel...\r\n");
	//	if (segment == 0x2000) {
	//		printstr("0x2000\r\n");
	//	}
	//	else if (segment == 0x3000) {
	//		printstr("0x3000\r\n");
	//	}
	//	else if (segment == 0x4000) {
	//		printstr("0x4000\r\n");
	//	}
	//	else if (segment == 0x5000) {
	//		printstr("0x5000\r\n");
	//	}
		unregister_proc(pid);
	}
	release_seg(segment);

	return retcode;
}

void set_scheduler() {
	int offset = 0x5;
	int segment = 0x1000;

	save_oldint8();


	//setting rml to be ISR for INT 08h

	__asm__ __volatile__ (
	  //first segment, where IVT is found
	  "  push %%ax\n"
	  "  push %%es\n"
	  "  push %%bx\n"
	  "  mov  $0x0, %%ax\n"
	  "  mov  %%ax, %%es\n"

	  //each entry in the IVT is 4 bytes. two bytes for offset and 2 bytes for segment
	  //08h * 04h = 20h
	  "  mov  $0x20, %%bx\n"
	  //offset where ISR is found (6h)
	  "  mov  %0, %%ax\n"
	  "  movw %%ax, %%es:(%%bx)\n"

	  //at the next two bytes (22h) resides the segment for the interupt handler
	  //segment where ISR is found (1000h - second segment)
	  "  mov  $0x22, %%bx\n"
	  "  mov  %1, %%ax\n"
	  "  movw %%ax, %%es:(%%bx)\n"
	  "  pop  %%bx\n"
	  "  pop  %%es\n"
	  "  pop  %%ax\n"
	  :
	  :"m"(offset), "m"(segment)
	  :"memory", "esi", "edi", "eax", "ebx", "ecx", "edx"
	);


	//update
	//outb(0x20,0x00);

	//end of interrupt
	//outb(0x20,0x20);
}

void PIC_sendEOI(unsigned char irq) {
	if(irq >= 8)
		outb(PIC2_COMMAND,PIC_EOI);

	outb(PIC1_COMMAND,PIC_EOI);
}

void save_oldint8() {
	unsigned short offs;
	unsigned short segment;

	__asm__ __volatile__ (
	  "  push %%ax\n"
	  "  push %%es\n"
	  "  push %%bx\n"

	  //first segment, where IVT is found
	  "  mov  $0x0, %%ax\n"
	  "  mov  %%ax, %%es\n"

	  //offset where ISR is found (20h)
	  //each entry in the IVT is 4 bytes. two bytes for offset and 2 bytes for segment
	  //08h * 04h = 20h
	  //at the next two bytes (22h) resides the segment for the interupt handler

	  // offset
	  "  mov  $0x20, %%bx\n"
	  "  movw %%es:(%%bx), %%ax\n"
	  "  mov  %%ax, %0\n"
	  //segment
	  "  add  $0x2, %%bx\n"
	  "  movw %%es:(%%bx), %%ax\n"
	  "  mov  %%ax, %1\n"

	  "  pop  %%bx\n"
	  "  pop  %%es\n"
	  "  pop  %%ax\n"
	  :"=m"(offs), "=m"(segment)
	  :
	  :"memory", "esi", "edi", "eax", "ebx", "ecx", "edx"
	);

	b_int8_offs = offs;
	b_int8_seg = segment;
	//printk("offset:");
	//printk(myitoa(offs));
	//printk("segment:");
	//printk(myitoa(segment));
}

void time() {
	unsigned int hours,minutes,seconds,year,month,day;

	read_RTC_date(&year,&month,&day);
	printk(uitoa(year));
	printk("/");
	printk(uitoa(month));
	printk("/");
	printk(uitoa(day));
	printk(" ");
	read_RTC_time(&hours,&minutes,&seconds);
	printk(uitoa(hours));
	printk(":");
	printk(uitoa(minutes));
	printk(":");
	printk(uitoa(seconds));
	printk("\r\n");

}
