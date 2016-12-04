#ifndef _BIOS_H
#define _BIOS_H

int read_sectors(unsigned int segment,int buf_offset, char num_sectors,char cylinder,char sector, char head, char drive);
void b_putchar(char in);
char b_getchar();
void reboot();
void read_RTC_time(unsigned int *out_hours,unsigned int *out_minutes,unsigned int *out_seconds);
void read_RTC_date(unsigned int *out_year,unsigned int *out_month,unsigned int *out_day);
unsigned char b_query_kb_stat();
char b_getchar_nonblock();

#endif
