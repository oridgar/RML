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

void printf(char *string) {
	printstr(string);
}

void printstr(char *string) {
	int i = 0;
	while (string[i] != 0) {
		_putchar(string[i]);
		i++;
	}
}

//-------------------------
//CPU INSTRUCTIONS WRAPPERS
//-------------------------

void halt() {
	asm {
		hlt
	}
}

void cli() {
	asm {
		cli
	}
}

void sti() {
	asm {
		sti
	}
}

void outb(int port, char value) {
	asm {
		push ax
		push dx
		mov dx,[port]
		mov al,[value]
		out dx,al
		pop dx
		pop ax
	}
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
	printstr("starting kernel...\r\n");
	init_seg();
	set_ivt();
	set_scheduler();
	//sti();

	mount("/dev/fdb");
	sti();
	//while (1) { run_init(); }
	while (1) {
		init_processes();
		run_program("init");
	}
}

void _dispatch(int service,SYSCALL_PARAM *params) {
	int curr_pid;
	int fd;

	//printk("We're in dispatch\r\n");
	switch (service) {
		case 0:
			printk("system call!\r\n");
			break;
		case 1:
			halt();
			break;
		case 2:
			reboot();
			break;
		case 3:
			//TODO: change from _getchar() to read()
			//read(0,params->param,1);
			params->param[0] = _getchar();
			break;
		case 4:
			printk(params->param);
			break;
		case 5:
			//TODO: change from _putchar() to write()
			_putchar(params->param[0]);
			break;
		case 6:
			//TODO: change from _getchar() to read()
			//read(0,params->param,1);
			params->param[0] = _getchar();
			break;
		case 7:
			//run_shell();
			curr_pid = get_running_proc();
			*(int *)params->param = run_program(params->param);
			set_running_proc(curr_pid);
			break;
		case 8:
			get_process_list();
			break;
		case 9:
			//fd = open("/",0);
			//lseek(fd,0,0);
			list_root_files(32,FILE_OUT_LIST);
			break;
		/*
		case 10:
			//open()
			break;
		case 11:
			//close()
			break;
		case 12:
			//seek()
			break;
		case 13:
			//read()
			break;
		case 14:
			//write()
			break;
		*/
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
	asm {
		mov  dl,[drive] //drive
		mov	 ch,[cylinder] // cylinder/track 
		mov  dh,[head] //head 0
		mov  cl,[sector] 
		mov  al,[num_sectors] //number of sectors to read
		mov  bx,[segment]
		mov  es,bx  //second 64K segment (pass by value)
		mov  bx,[prog_offset] //load to prog_offset from segment
		mov  ah,0x02 //Read Sectors From Drive service
		int  13h
	}
}

void uname() {
	return;
}

void set_ivt() {
	asm {
			//first segment, where IVT is found
			mov ax,0h
			mov es,ax
						
			//offset where ISR is found (0h)
			//each entry in the IVT is 4 bytes. two bytes for offset and 2 bytes for segment
			//80h * 04h = 200h
			//at the next two bytes (202h) resides the segment for the interupt handler
			mov bx,200h
			mov ax,0h
			mov es:[bx],ax
			
			//segment where ISR is found (1000h - second segment)
			mov bx,202h
			mov ax,1000h
			mov es:[bx],ax
	}
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
	save_oldint8();


	//setting rml to be ISR for INT 08h

	asm {
			//first segment, where IVT is found
			push ax
			push es
			push bx
			mov ax,0h
			mov es,ax

			//each entry in the IVT is 4 bytes. two bytes for offset and 2 bytes for segment
			//08h * 04h = 20h
			mov bx,20h
			//offset where ISR is found (6h)
			mov ax,5h
			mov es:[bx],ax

			//at the next two bytes (22h) resides the segment for the interupt handler
			//segment where ISR is found (1000h - second segment)
			mov bx,22h
			mov ax,1000h
			mov es:[bx],ax
			pop bx
			pop es
			pop ax
	}


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
	unsigned int offs;
	unsigned int segment;

	asm {

		push ax
		push es
		push bx

		//first segment, where IVT is found
		mov ax,0h
		mov es,ax

		//offset where ISR is found (20h)
		//each entry in the IVT is 4 bytes. two bytes for offset and 2 bytes for segment
		//08h * 04h = 20h
		//at the next two bytes (22h) resides the segment for the interupt handler

		// offset
		mov bx,20h
		mov ax,es:[bx]
		mov [offs],ax
		//segment
		add bx,2
		mov ax,es:[bx]
		mov [segment],ax

		pop bx
		pop es
		pop ax
	}

	b_int8_offs = offs;
	b_int8_seg = segment;
	//printk("offset:");
	//printk(myitoa(offs));
	//printk("segment:");
	//printk(myitoa(segment));
}
