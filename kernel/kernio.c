#include "linux/kernio.h"
#include "bios.h"
#include "fs/fs.h"

FileDescriptor fd_list[5];


int lba_to_chs(off_t offset, char cylinders, char heads, char sectors, char *cylinder, char *head, char *sector) {
	*sector = (offset / 512) % sectors + 1;
	*head = ((offset / 512) + 1) / sectors % heads;
	*cylinder = ((offset / 512)) / (sectors * heads);
	return 0;
}

int open(const char *pathname, int flags) {
	if (strcmp(pathname,"/dev/fda") == 0) {
		//strcpy(fd_list[3].pathname,"/dev/fda");
		fd_list[3].pos = 0;
		return 3;
	}
	else if (strcmp(pathname,"/dev/fdb") == 0) {
		fd_list[4].pos = 0;
		return 4;
	}
	return -1;
}

int close(int fd) {
	return 0;
}

off_t lseek(int fd, off_t offset, int whence) {
	fd_list[fd].pos = offset;
	return offset;
}

unsigned int read(int fd, void *buf, unsigned int count) {
	char cylinder;
	char head;
	char sector;
	int  num_sectors;
	int  num_bytes;
	char drive;
	char drive_num_cylinder;
	char drive_num_heads;
	char drive_num_sectors;
	char sector_buf[512];
	int  segment = 0x1000;
	int  sector_offset;
	char *new_buf;

	new_buf = (char *) buf;
	//DEBUG
	//printk("entered to read \r\n");
	//END DEBUG
	if (fd == 3) {
		drive = 0x00;
		drive_num_cylinder = 80;
		drive_num_heads = 2;
		drive_num_sectors = 18;
		//floppy a
	}
	else if (fd == 4) {
		drive = 0x01;
		drive_num_cylinder = 80;
		drive_num_heads = 2;
		drive_num_sectors = 18;
		//floppy b
	}

	//converting lba to chs
	lba_to_chs(fd_list[fd].pos,drive_num_cylinder,drive_num_heads,drive_num_sectors,&cylinder,&head,&sector);
	if (count == 0) {
		num_sectors = 0;
	}
	else {
		num_sectors = (count - 1) / 512 + 1;
	}
	num_bytes = count;
	sector_offset = fd_list[fd].pos % 512;

	//each iteration, reading from current cylinder and head
	while (num_sectors > 0) {
		//moves to the next head
		if (sector > 18 ) {
			//read (18 - sector + 1) sectors
			sector = 1;
			head = (head + 1) % 2;
			if (head == 0) {
				cylinder++;
			}
			//num_sectors -= 18 - sector + 1; //decrement number of sectors read already
		}
		//DEBUG
//		printk("reading cylinder: ");
//		printk(myitoa((int)cylinder));
//		printk(" head: ");
//		printk(myitoa((int)head));
//		printk(" sector: ");
//		printk(myitoa((int)sector));

		//END DEBUG
		//reading into kernel segment
		if (read_sectors(segment,(int)sector_buf, 1 ,cylinder,sector, head, drive)) {
			printk("error reading sectors!\r\n");
		}

//		printk(" to address:");
//		//printk(myitoa((unsigned int)buf));
//		//printk(" new_buf:");
//		printk(myitoa((unsigned int)new_buf));
//		printk("\r\n");

		if (num_bytes > 512) {
			memcpy(new_buf,sector_buf+sector_offset,512);
			num_bytes -= 512 - sector_offset;
		}
		else {
			memcpy(new_buf,sector_buf+sector_offset,num_bytes);
			num_bytes = 0;
		}
		//(char)buf += 512 - sector_offset;
		new_buf += 512 - sector_offset;
		num_sectors--;
		sector++;
		sector_offset = 0;
	}

	fd_list[fd].pos += count;
	return 0;
}


void printk(char *string) {
	int i = 0;
	while (string[i] != 0) {
		b_putchar(string[i]);
		i++;
	}
}

char *myitoa(unsigned int i) {
	static char  buf[5 + 2];
	char *p = buf + 5 + 1;
	if (i >= 0) {
		do {
			*--p = '0' + (i % 10);
			i /= 10;
		} while (i != 0);
		return p;
	}
	else {
		do {
			*--p = '0' - (i % 10);
			i /= 10;
		} while (i != 0);
		*--p = '-';
	}
	return p;
}

int mount(char * filename) {
	//FILESYSTEM VARIABLES
	char fat_table[1024];
    //char root_dir_content[7168];
    Fat16Entry file_list[256];
    Fat16Entry currfile;
    int fd;
    int retcode;

	//FILESYSTEM CODE
    fd = open_drive();

    //printk("reading boot sector\r\n");
    read_boot_sector();

    //printk("reading fat\r\n");
    read_fat(fat_table);

    //printk("reading root directory content\r\n");
    //read_root_dir(root_dir_content);
    //printk("media descriptor byte: ");
    //switch (bs.media_descriptor) {
    //case 0xf0:
    	//printk("floppy 1440KB\r\n");
    	//break;
    //}

    //caching and listing all file metadatas
    //printk("get root files\r\n");
    get_root_files(file_list);

//    printk("after get_root_files \r\n");
//    printk("file size:");
//    printk(myitoa(((int)file_list[2].file_size)));
//    printk("\r\n");
//    printk("starting cluster:");
//    printk(myitoa(file_list[2].starting_cluster));
//    printk("\r\n");


//    printk("printing file list\r\n");
    //printing results
    //----------------
    list_files(file_list);

//    printk("\r\n");

//    printk("after list_files \r\n");
//    printk("file size:");
//	printk(myitoa(((int)file_list[2].file_size)));
//	printk("\r\n");
//	printk("starting cluster:");
//	printk(myitoa(file_list[2].starting_cluster));
//	printk("\r\n");

    retcode = get_file(file_list,"FDSPTCH",&currfile);

//    printk("after get_file \r\n");
//    printk("file size:");
//    printk(myitoa(((int)file_list[2].file_size)));
//    printk("\r\n");
//    printk("starting cluster:");
//    printk(myitoa(file_list[2].starting_cluster));
//    printk("\r\n");

    if (retcode == 0) {
    	printk("printing file content of FDSPTCH:\r\n\r\n");
    	//printk(currfile.filename);
    	print_file(&currfile);
    }
    else {
    	printk("file not found");
    }
	//fclose(drv);

	//END OF FILESYSTEM
	return 0;
}

void load_to_memory(unsigned int segment,int prog_offset,char *buffer,int count) {
	count = count /2;
	asm {
		push di
		push ax
		push es
		push cx

		mov  si,[buffer]
		//ds already configured...
		mov  di,[prog_offset] // destination of copy is params global variable in kernel
		mov	 es,[segment]

		//;-------------
		//;copy function
		//;-------------
		mov cx,[count]
		cld // direction is forward
	}
cpy:
	asm {
		lodsw // copy next word to AX : LODSW (DS:SI -> AX) and SI++
		stosw // copy next word from AX : STOSW (AX -> ES:DI) and DI++
		loop cpy
		pop cx
		pop es
		pop ax
		pop di
	}
}
