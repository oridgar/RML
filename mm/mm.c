#include <linux/mm.h>

MEM_SEG seg_arr[NUM_SEG];

void init_seg() {
	seg_arr[0].used = 1; //IVT, boot loader and BIOS data
	seg_arr[0].address = 0x0000;
	
	seg_arr[1].used = 1; //kernel
	seg_arr[1].address = 0x1000;
	
	seg_arr[2].used = 0; 
	seg_arr[2].address = 0x2000;
	
	seg_arr[3].used = 0; //OLD stack
	seg_arr[3].address = 0x3000;
	
	seg_arr[4].used = 0;
	seg_arr[4].address = 0x4000;
	
	seg_arr[5].used = 0;
	seg_arr[5].address = 0x5000;
	
	seg_arr[6].used = 0;
	seg_arr[6].address = 0x6000;
	
	seg_arr[7].used = 0;
	seg_arr[7].address = 0x7000;
	
	seg_arr[8].used = 0;
	seg_arr[8].address = 0x8000;
	
	seg_arr[9].used = 0;
	seg_arr[9].address = 0x9000;
}

unsigned int get_free_seg() {
	int i;
	unsigned int seg;
	for (i=0; i < NUM_SEG; i++) {
		if (seg_arr[i].used == 0) {
			seg = seg_arr[i].address;
			seg_arr[i].used = 1;
			break;
		}
	}
	return seg;
}

unsigned int release_seg(unsigned int seg) {
	int i;
	//char num;
	for (i=0; i < NUM_SEG; i++) {
		
		//printstr("checking segment number ");
		//num = '1' + i;
		//_putchar(num);
		//printstr("...");
		if (seg_arr[i].address == seg) {
			//printstr("released!\r\n");
			//printstr('0' + (char)i);
			seg_arr[i].used = 0;
			//printstr("released!\r\n");
			break;
		}
		//printstr("\r\n");
	}
	return i;
}
