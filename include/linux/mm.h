#define NUM_SEG 10

int get_free_seg();
void init_seg();

typedef struct 
{
  char used;
  int  address;
}MEM_SEG ;