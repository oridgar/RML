#include "sysdef.h"
#ifndef MM
#include "linux/mm.h"
#define MM
#endif
#ifndef STRING
#include "mystring.h"
#define STRING
#endif
#include "fs/fs.h"
#include "types.h"
#include "bios.h"
#include "linux/kernio.h"

//PIC
#define PIC1	0x20
#define PIC2	0xA0
#define PIC1_COMMAND	PIC1
#define PIC1_DATA	(PIC1+1)
#define PIC2_COMMAND	PIC2
#define PIC2_DATA	(PIC2+1)
#define PIC_EOI	0x20

//KERNEL SERVICES - PUBLIC
void reboot();
void halt();
void uname();
void run_program(char *name); //run program = load + call
//void run_init();
//void run_shell();

//KERNEL SERVICES - PRIVATE
unsigned int load(char *name); //loading program into memory
void outb(int port, char value);
void set_scheduler();
extern void farcall(int seg,int ofs); //call the program

extern int b_int8_seg;
extern int b_int8_offs;

//CPU RELATED
void cli();
void sti();
void set_ivt();

//BIOS RELATED
//void clear_scr();
void printstr(char *string);
void _putchar(char in);
char _getchar();
void load_program(unsigned int segment,int prog_offset, char num_sectors,char cylinder,char sector, char head, char drive);
void save_oldint8();

void printf(char *string) {
	printstr(string);
}

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

	mount("a");
	sti();
	//while (1) { run_init(); }
	while (1) { run_program("init"); }
}

//==============
//BIOS FUNCTIONS
//==============

char _getchar() {
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

void printstr(char *string) {
	int i = 0;
	while (string[i] != 0) {
		_putchar(string[i]);
		i++;
	}
}

void _putchar(char in) {
	char temp = in;
	asm {
		  mov ah,0Eh
		  mov al,[temp]
		  int 10h
	}
}

//==================
//END BIOS FUNCTIONS
//==================

void _dispatch(int service,SYSCALL_PARAM *params) {
	switch (service) {
		case 0:
			printstr("system call!\r\n");
			break;
		case 1:
			halt();
			break;
		case 2:
			reboot();
			break;
		case 3:
			params->param[0] = _getchar();
			break;
		case 4:
			printstr(params->param);
			break;
		case 5:
			_putchar(params->param[0]);
			break;
		case 6:
			params->param[0] = _getchar();
			break;
		case 7:
			//run_shell();
			run_program(params->param);
			break;
		default:
			break;
	}
	return;
}

unsigned int load(char *name) {
	unsigned int segment = get_free_seg();
	int offset = 0x0000;
	char num_sectors;
	char cylinder;
	char sector;
	char head;
	char drive;
	Fat16Entry file_list[256];
	Fat16Entry currfile;
	
	//replace it with get_file and get_file_content
	if (strcmp(name,"init") == 0) {
		num_sectors = 0x12;
		drive = 0x00;
		cylinder = 0x00;
		head = 0x01;
		sector = 0x01;
		load_program(segment,offset,num_sectors,cylinder,sector,head,drive);
	}
	else if (strcmp(name,"shell") == 0) {
		num_sectors = 0x12;
		drive = 0x00;
		cylinder = 0x01;
		head = 0x00;
		sector = 0x01;
		load_program(segment,offset,num_sectors,cylinder,sector,head,drive);
	}
	else if (strcmp(name,"ls") == 0) {
		get_root_files(file_list);
		if (get_file(file_list,"LS",&currfile) == -1) {
			printk("could not find file!!!\r\n");
		}
		else {
			load_file_content(&currfile,segment,offset);
		}
		//num_sectors = 0x12;
		//drive = 0x00;
		//cylinder = 0x01;
		//head = 0x01;
		//sector = 0x01;
	}
	//TODO: to be able to return another code that indicate that the program is not found!
	
	return segment;
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
		mov  bx,[prog_offset] //load to 0h (binary file org 0 instead of com file) TODO: fix taking from offset param
		mov  ah,0x02 //Read Sectors From Drive service
		int  13h
	}
}

void reboot() {
	asm {
		int 19h
	}
}

void halt() {
	asm {
		hlt
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

//void run_init() {
//	run_program("init");
//}

//void run_shell() {
	//run_program("shell");
//}

//running a program by name
//calling load to load the program into memory
//calling farcall to run the program
void run_program(char *name) {
	unsigned int segment;
	char seg_prefix;
	printstr("loading ");
	printstr(name);
	printstr("...\r\n");
	segment = load(name);

	printstr("loaded to 0x");
	seg_prefix = '0' + (segment / 0x1000);
	_putchar(seg_prefix);
	printstr("000\r\n");
	
	//For now supporting only binary format. code origin is in 0x0000 or 0h
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
	release_seg(segment);
		
	return;
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
