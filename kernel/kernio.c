#include <linux/kernio.h>
#include <bios.h>
#include <fs/fs.h>
#include <linux/sched.h>
#include <string.h>

FileDescriptor fd_list[5]; //For the kernel itself

int lba_to_chs(unsigned int lba, unsigned char cylinders, unsigned char heads, unsigned char sectors, unsigned char *cylinder, unsigned char *head, unsigned char *sector) {
	*sector = lba % sectors + 1;
	*head = (lba + 1) / sectors % heads;
	*cylinder = (lba) / (sectors * heads);
	return 0;
}

int open(const char *pathname, int flags,int pid) {
//int open(const char *pathname, int flags) {
	FileDescriptor *curr_fd_list;
	int fd = -1;

	//TODO: change the file descriptor table from global wide to process wide
	if (pid == 0) {
		curr_fd_list = fd_list;
	}
	else {
		curr_fd_list = processes[pid-1].files_table;
	}

	if (strcmp(pathname,"/dev/fda") == 0) {
		fd = 3;
		//strcpy(fd_list[3].pathname,"/dev/fda");
		curr_fd_list[fd].pos = 0;
		curr_fd_list[fd].pfd = -1;
		curr_fd_list[fd].size = 65535;
		//return 3;
	}
	//TODO: FIX THIS BUG!!!
	else if (strcmp(pathname,"/dev/stdin") == 0) {
		fd = 0;
		curr_fd_list[fd].pos = 0;
		curr_fd_list[fd].size = 0;
		curr_fd_list[fd].pfd = -1;
		curr_fd_list[fd].pfd_offset = 0;
		//return 0;
	}
	else if (strcmp(pathname,"/dev/stdout") == 0) {
		fd = 1;
		curr_fd_list[fd].pos = 0;
		curr_fd_list[fd].size = 0;
		curr_fd_list[fd].pfd = -1;
		curr_fd_list[fd].pfd_offset = 0;
		//return 0;
	}
//	else if (strcmp(pathname,"/dev/stderr") == 0) {
//		fd = 2;
//		curr_fd_list[fd].pos = 0;
//		curr_fd_list[fd].size = 0;
//		curr_fd_list[fd].pfd = -1;
//		curr_fd_list[fd].pfd_offset = 0;
//		//return 0;
//	}
	else if (strcmp(pathname,"/dev/fdb") == 0) {
		fd = 4;
		curr_fd_list[fd].pos = 0;
		curr_fd_list[fd].pfd = -1;
		curr_fd_list[fd].size = 65535;
		//return 4;
	}
	else if (strcmp(pathname,"/") == 0) {
		fd = 5;
		curr_fd_list[fd].pos = 0;
		curr_fd_list[fd].size = get_root_size();
		curr_fd_list[fd].pfd = 4;
		curr_fd_list[fd].pfd_offset = get_root_offset();
		//return 5;
	}
	return fd;
}

//TODO: change to int close(int fd,int pid) {
int close(int fd,int pid) {
	FileDescriptor *curr_fd_list;

	//TODO: change the file descriptor table from global wide to process wide
	if (pid == 0) {
		curr_fd_list = fd_list;
	}
	else {
		curr_fd_list = processes[pid-1].files_table;
	}

	curr_fd_list[fd].pathname[0] = '\0';
	curr_fd_list[fd].pfd = 0;
	curr_fd_list[fd].pfd_offset = 0;
	curr_fd_list[fd].pos = 0;
	curr_fd_list[fd].size = 0;
	return 0;
}

//TODO: change to off_t lseek(int fd, off_t offset, int whence,int pid)
off_t lseek(int fd, off_t offset, int whence,int pid) {
	FileDescriptor *curr_fd_list;

	//TODO: change the file descriptor table from global wide to process wide
	if (pid == 0) {
		curr_fd_list = fd_list;
	}
	else {
		curr_fd_list = processes[pid-1].files_table;
	}

	curr_fd_list[fd].pos = offset;
	return offset;
}

//TODO: change to unsigned int read(int fd, void *buf, unsigned int count,int pid)
unsigned int read(int fd, void *buf, unsigned int count,int pid) {
	unsigned char cylinder;
	unsigned char head;
	unsigned char sector;
	unsigned int  num_sectors;
	unsigned int  num_bytes;
	char drive;
	char drive_num_cylinder;
	char drive_num_heads;
	char drive_num_sectors;
	char sector_buf[512];
	int  segment = 0x1000;
	unsigned int  sector_offset;
	char *new_buf;
	int  i;
	int bytes_read;
	FileDescriptor *curr_fd_list;

	//TODO: change the file descriptor table from global wide to process wide
	if (pid == 0) {
		curr_fd_list = fd_list;
	}
	else {
		curr_fd_list = processes[pid-1].files_table;
	}

	new_buf = (char *) buf;
	//DEBUG
	//printk("entered to read \r\n");
//	if (fd == 5) {
//		printk("reading fd: ");
//		printk(myitoa(fd));
//		printk(" pfd= ");
//		printk(myitoa(fd_list[fd].pfd));
//		printk(" pfd_offset= ");
//		printk(myitoa(fd_list[fd].pfd_offset));
//		printk(" pos= ");
//		printk(myitoa(fd_list[fd].pos));
//
//		printk("\r\n");
//	}

	if (fd_list[fd].size <=  fd_list[fd].pos) {
		//need to return EOF

		//to calculate left bytes to read
		//fd_list[fd].size - fd_list[fd].pos
	}

	//END DEBUG
	//while there are device loops
	if (fd_list[fd].pfd != -1) {
		lseek(fd_list[fd].pfd,fd_list[fd].pfd_offset + fd_list[fd].pos,0,pid);
		bytes_read = read(fd_list[fd].pfd,new_buf,count,0);
	}
	//the device is direct/hardware block/character device
	else if (fd == 3 || fd == 4) {
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
		lba_to_chs((fd_list[fd].pos / 512),drive_num_cylinder,drive_num_heads,drive_num_sectors,&cylinder,&head,&sector);
//		printk("starting cylinder: ");
//		printk(uitoa((unsigned int)cylinder));
//		printk(" starting head: ");
//		printk(uitoa((unsigned int)head));
//		printk(" starting sector: ");
//		printk(uitoa((unsigned int)sector));
//		printk("\r\n");
		if (count == 0) {
			num_sectors = 0;
		}
		else {
			num_sectors = (count - 1) / 512 + 1;
		}
//		printk("num_sectors = ");
//		printk(uitoa(num_sectors));
		num_bytes = count;
//		printk(" num_bytes = ");
//		printk(uitoa(num_bytes));
		sector_offset = fd_list[fd].pos % 512;
//		printk(" sector_offset = ");
//		printk(uitoa(num_bytes));
//		printk("\r\n");
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
//			printk("reading cylinder: ");
//			printk(uitoa((unsigned int)cylinder));
//			printk(" head: ");
//			printk(uitoa((unsigned int)head));
//			printk(" sector: ");
//			printk(uitoa((unsigned int)sector));
//			printk("\r\n");

			//END DEBUG
			//reading into kernel segment
			if (read_sectors(segment,(int)sector_buf, 1 ,cylinder,sector, head, drive)) {
				printk("error reading sectors!\r\n");
				halt();
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
		bytes_read = count;
	}
	else if (fd == 0) {
		for(i=0;i < count; i++) {
			//((char *)buf)[i] = b_getchar();
			((char *)buf)[i] = b_getchar_sync();
		}
		bytes_read = 0;
	}
	//TODO: change the position only if this is not character device
	fd_list[fd].pos += bytes_read;
	return bytes_read;
}

int write(int fd, void *buf, unsigned int count,int pid) {
	int i;
	FileDescriptor *curr_fd_list;

	//TODO: change the file descriptor table from global wide to process wide
	if (pid == 0) {
		curr_fd_list = fd_list;
	}
	else {
		curr_fd_list = processes[pid-1].files_table;
	}

	//char *msg = "write function!!!\r\n";

	//while(*msg != '\0') {
//		b_putchar(*msg);
//		msg++;
//	}

	if (fd == 1 || fd == 2) {
		for(i=0;i < count; i++) {
			b_putchar(((char *)buf)[i]);
		}
		return count;
	}
	return count;
}


void printk(char *string) {
	write(1,string,strlen(string),0);
}

char *uitoa(unsigned int i) {
	static char  buf[5 + 2];
	char *p = buf + 5 + 1;
	//TODO: check that putting \0 to the end of buffer works
	//p[6] = '\0';
	//if (i >= 0) {
		do {
			*--p = '0' + (i % 10);
			i /= 10;
		} while (i != 0);
		return p;
	//}
//	else {
//		do {
//			*--p = '0' - (i % 10);
//			i /= 10;
//		} while (i != 0);
//		*--p = '-';
//	}
//	return p;
}

int mount(char * filename) {
	//FILESYSTEM VARIABLES
	char fat_table[1024];
    //char root_dir_content[7168];
    Fat16Entry file_list[256];
    Fat16Entry currfile;
    //int fd;
    int retcode;

    if(strcmp(filename,"/dev/fdb") == 0) {
		//FILESYSTEM CODE
		//fd = open_drive();
		open_drive();

		//printk("reading boot sector\r\n");
		read_boot_sector();

		//printk("reading fat\r\n");
		read_fat(fat_table);

		//opening root filesystem
		open("/",0,0);
    }

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
    //get_root_files(file_list);

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
    //list_files(file_list);

//    printk("\r\n");

//    printk("after list_files \r\n");
//    printk("file size:");
//	printk(myitoa(((int)file_list[2].file_size)));
//	printk("\r\n");
//	printk("starting cluster:");
//	printk(myitoa(file_list[2].starting_cluster));
//	printk("\r\n");

    //retcode = get_file(file_list,"FDSPTCH",&currfile);

//    printk("after get_file \r\n");
//    printk("file size:");
//    printk(myitoa(((int)file_list[2].file_size)));
//    printk("\r\n");
//    printk("starting cluster:");
//    printk(myitoa(file_list[2].starting_cluster));
//    printk("\r\n");

    //if (retcode == 0) {
    	//printk("printing file content of FDSPTCH:\r\n\r\n");
    	//printk(currfile.filename);
    	//print_file(&currfile);
    //}
    //else {
    	//printk("file not found");
    //}
	//fclose(drv);

	//END OF FILESYSTEM
	return 0;
}

void load_to_memory(unsigned int segment,int prog_offset,char *buffer,int count) {
	count = count /2;
	__asm__ (
		"push di\n\t"
		"push ax\n\t"
		"push es\n\t"
		"push cx\n\t"

		"mov  si,[%0]\n\t"
		//ds already configured...
		"mov  di,[%1]\n\t" // destination of copy is params global variable in kernel
		"mov  es,[%2]\n\t"

		//;-------------
		//;copy function
		//;-------------
		"mov cx,%3\n\t"
		"cld\n" // direction is forward
		"cpy:\n\t"
		"lodsw\n\t" // copy next word to AX : LODSW (DS:SI -> AX) and SI++
		"stosw\n\t" // copy next word from AX : STOSW (AX -> ES:DI) and DI++
		"loop cpy\n\t"
		"pop cx\n\t"
		"pop es\n\t"
		"pop ax\n\t"
		"pop di\n\t"
		:
		: "r" (buffer), "r" (prog_offset), "r" (segment), "r" ((short)count)
	);
}

//int getdents(unsigned int fd, linux_dirent *dirp, unsigned int count) {
	//return 0;
//}
