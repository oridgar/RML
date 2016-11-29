#ifndef _MM_H
#define _MM_H

#define NUM_SEG 10

unsigned int get_free_seg();
void init_seg();
unsigned int release_seg(unsigned int seg);

typedef struct 
{
  char used;
  unsigned int  address;
}MEM_SEG ;

#endif
