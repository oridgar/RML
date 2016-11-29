#include "sysdef.h"
#ifndef MM
#include "linux/mm.h"
#define MM
#endif
#ifndef STRING
#include "mystring.h"
#define STRING
#endif

//KERNEL SERVICES - PUBLIC
void reboot();
void halt();
void uname();
void run_program(char *name); //run program = load + call
//void run_init();
//void run_shell();

//KERNEL SERVICES - PRIVATE
int load(char *name); //loading program into memory
extern void farcall(int seg,int ofs); //call the program

//CPU RELATED
void cli();
void sti();
void set_ivt();

//BIOS RELATED
void clear_scr();
void printstr(char *string);
void _putchar(char in);
char _getchar();
void load_program(int segment,int prog_offset, char num_sectors,char cylinder,char sector, char head, char drive);



void startk() {
	int i;
	printstr("starting kernel...\r\n");
	init_seg();
	set_ivt();
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

int load(char *name) {
	int segment = get_free_seg();
	int offset = 0x0000;
	char num_sectors;
	char cylinder;
	char sector;
	char head;
	char drive;
	
	if (strcmp(name,"init") != -1) {
		num_sectors = 0x12;
		drive = 0x00;
		cylinder = 0x00;
		head = 0x01;
		sector = 0x01;
	}
	else if (strcmp(name,"shell") != -1) {
		num_sectors = 0x12;
		drive = 0x00;
		cylinder = 0x01;
		head = 0x00;
		sector = 0x01;
	}
	
	load_program(segment,offset,num_sectors,cylinder,sector,head,drive);
		
	return segment;
}

void load_program(int segment,int prog_offset, char num_sectors,char cylinder,char sector, char head, char drive) {
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
	int segment;
	printstr("loading ");
	printstr(name);
	printstr("...\r\n");
	segment = load(name);

	if (segment == 0x2000) {
		printstr("loaded to 0x2000\r\n");
	}
	else if (segment == 0x3000) {
		printstr("loaded to 0x3000\r\n");
	}
	else if (segment == 0x4000) {
		printstr("loaded to 0x4000\r\n");
	}
	else if (segment == 0x5000) {
		printstr("loaded to 0x5000\r\n");
	}
	
	farcall(segment,0x0000);
	/*asm {
				db 9Ah // CALL FAR instruction
			  //db  0eah //JMP FAR instruction 
				dw  0 //offset
				dw  2000h //segment TODO:get it by parameter and not hard coded!
	}*/
	
	//macro example
	/*
	FARCALL macro seg, ofs 
        db 9Ah  ; CALL FAR instruction 
        dw ofs 
        dw seg 
	endm 
        And then use 
	FARCALL 0043h, 0130h 
	*/

	printstr("returned to kernel...\r\n");
	if (segment == 0x2000) {
		printstr("0x2000\r\n");
	}
	else if (segment == 0x3000) {
		printstr("0x3000\r\n");
	}
	else if (segment == 0x4000) {
		printstr("0x4000\r\n");
	}
	else if (segment == 0x5000) {
		printstr("0x5000\r\n");
	}
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