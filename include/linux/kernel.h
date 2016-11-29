/*
 * kernel.h
 *
 *  Created on: Nov 27, 2016
 *      Author: idgaro
 */

#ifndef _KERNEL_H
#define _KERNEL_H

//PIC
#define PIC1	0x20
#define PIC2	0xA0
#define PIC1_COMMAND	PIC1
#define PIC1_DATA	(PIC1+1)
#define PIC2_COMMAND	PIC2
#define PIC2_DATA	(PIC2+1)
#define PIC_EOI	0x20

//KERNEL SERVICES - PUBLIC
//void reboot();
void halt();
void uname();
int run_program(char *name); //run program = load + call
//void run_init();
//void run_shell();

//KERNEL SERVICES - PRIVATE
unsigned int load(char *name,int *pid,unsigned int *seg); //loading program into memory
void outb(int port, char value);
void set_scheduler();

//CPU RELATED
void cli();
void sti();
void set_ivt();

//BIOS RELATED
//void clear_scr();
void printstr(char *string);
//void _putchar(char in);
//char _getchar();
void load_program(unsigned int segment,int prog_offset, char num_sectors,char cylinder,char sector, char head, char drive);
void save_oldint8();


#endif /* INCLUDE_LINUX_KERNEL_H_ */
