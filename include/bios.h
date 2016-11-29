#ifndef _BIOS_H
#define _BIOS_H

int read_sectors(unsigned int segment,int buf_offset, char num_sectors,char cylinder,char sector, char head, char drive);
void b_putchar(char in);
char _getchar();
void _putchar(char in);
void reboot();

#endif
