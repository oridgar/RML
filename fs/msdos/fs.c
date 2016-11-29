#include "mystring.h"
#include "fs/fs.h"
#include "linux/kernio.h"


//----------------
//global variables
//----------------

sectionPositions s_pos;
Fat16BootSector bs;
int drv;

int open_drive() {
	//FILL IN!
	//drv = fopen("/cygdrive/d/Code/fatfs/fat.bin","ab+");
	drv = open("/dev/fdb",0);
	return drv;
}

int read_boot_sector() {
	unsigned int  root_sectors;

	//reading the boot sector which includes BIOS Parameter Block + Extended BIOS Parameter Block
	lseek(4,0, 0);
	read(4, &bs, sizeof(Fat16BootSector));

    root_sectors = bs.root_dir_entries * 32 / bs.sector_size;


    s_pos.fat_section_start = bs.sector_size * bs.reserved_sectors;
    s_pos.root_directory_start = s_pos.fat_section_start + ((bs.sector_size) * (bs.fat_size_sectors) * (bs.number_of_fats));
    s_pos.data_section_start = s_pos.root_directory_start + (bs.sector_size * root_sectors);
    return 0;
}


void print_fs_params() {

}


int read_fat(char* fatcache) {
	//moving to position of next cluster entry in fat + reading
	//WORKING!
	//FILL IN!
	lseek(4,s_pos.fat_section_start, 0);
	read(4,fatcache, (bs.number_of_fats * bs.fat_size_sectors));

	//fseek(drv, s_pos.fat_section_start, SEEK_SET);
	//fread(fatcache, (bs.number_of_fats * bs.fat_size_sectors), 1, drv);
	return 0;
}



int read_root_dir(char *rootcache) {
	//seeking to the beginning of root directory area
	//FILL IN!
	lseek(4,s_pos.root_directory_start, 0);
	read(4,rootcache, sizeof(Fat16Entry) * bs.root_dir_entries);

	//fseek(drv, s_pos.root_directory_start , SEEK_SET);
	//fread(rootcache, sizeof(Fat16Entry) * bs.root_dir_entries, 1, drv);
	return 0;
}

int read_cluster(char *buffer,unsigned int cluster_num) {
	unsigned int  cluster_size;
	unsigned int  seek_pos;

	cluster_size = bs.sector_size * bs.sectors_per_cluster;

	//read file
	//note that the first two clusters contains special values. we just skip it.
	//in the FAT12 table we need to skip the first two clusters, hence three bytes because of 12 bit for entry. they contain the values F0 FF FF.
	//starting cluster -2 is how many clusters we should jump to get to desired cluster.
	//first usable cluster is 3 and it is one cluster offset away, so 3 - 1 = 2
	//remember that in the disk the cluster numbering starts from 0
	//cluster 0 - 1: reserved
	//cluster 2 : unused
	//cluster 3 : first cluster used
	//FILL IN!
	//fseek(drv, s_pos.data_section_start + (cluster_size * (cluster_num - 2))  , SEEK_SET);
	//fread(buffer, cluster_size, 1, drv);

	seek_pos = s_pos.data_section_start + (cluster_size * (cluster_num - 2));
	//DEBUG
	//printk("seek pos:");
	//printk(myitoa(seek_pos));
	//printk("\r\n");
	//END DEBUG
	lseek(4,seek_pos, 0);
	read(4,buffer, cluster_size);

	return 0;
}

int file_attrib(Fat16Entry *file,fileAttrib *attrib) {
	attrib->read_only = file->attributes & 0x01;
	attrib->hidden = file->attributes & 0x02;
	attrib->system_file = file->attributes & 0x04;
	attrib->volume_label = file->attributes & 0x08;
	attrib->subdirectory = file->attributes & 0x10;
	attrib->archive = file->attributes & 0x20;

	return 0;
}

int get_next_cluster(unsigned int curr_cluster,unsigned int *next_cluster) {
	unsigned int  cluster_entry;

	//----------------------------
	//CALCULATING THE NEXT CLUSTER
	//----------------------------
	//from where to read the next cluster value in FAT table (entries in the table starts from 0)
	//0,1 are reserved with some values. 2 is not used?
	cluster_entry = curr_cluster + (curr_cluster / 2);

	//moving to position of next cluster entry in fat + reading
	//WORKING!
	//FILL IN!
	//fseek(drv, s_pos.fat_section_start + cluster_entry, SEEK_SET);
	lseek(4,s_pos.fat_section_start + cluster_entry, 0);
	*next_cluster = 0;
	//fread(next_cluster, 2, 1, drv);
	read(4,next_cluster, 2);

	// if the current cluster is odd
	if (curr_cluster & 0x0001) {
		*next_cluster = *next_cluster >> 4; //taking the highest 3 nibbles
	}
	//current cluster is even
	else {
		*next_cluster = *next_cluster & 0x0FFF; //taking the lowest 3 nibbles
	}
	return 0;
}



int get_root_files(Fat16Entry *files) {
	int i;


	lseek(5,0,0);
	for(i=0; i < bs.root_dir_entries;i++) {
		read(5,&files[i], sizeof(Fat16Entry));
	}


	/*
	printk("reading root files. start pos: ");
	printk(myitoa(s_pos.root_directory_start));
	printk("\r\n");
	lseek(4,s_pos.root_directory_start, 0);

	for(i=0; i < bs.root_dir_entries;i++) {
		read(4,&files[i], sizeof(Fat16Entry));

	}
*/

	return 0;
}

int get_root_next_file(Fat16Entry *file) {
	read(5,file, sizeof(Fat16Entry));

	return 0;
}

int list_root_files(int size) {
	int i;
	int fd;
	int files_read = 0;
	Fat16Entry file;
	fileAttrib attrib;
	char name[9];
	char ext[4];

	// TODO: remove opening and seeking root folder and make sure get_root_next_file returns value that
	// indicates there are no more files
	fd = open("/",0);
	lseek(fd,0,0);
	// && sizeof(Fat16Entry)*files_read < size
	for (i=0; i < bs.root_dir_entries; i++) {
		get_root_next_file(&file);
		if (get_entry_type(&file) == TYPE_FILE) {
			//deleted file
			if (file.filename[0] == 0xe5) {
				continue;
			}
			//no file
			else if (file.filename[0] == 0x00) {
				break;
			}
			else {
				extract_file_name(&file,name,ext);
				printk(name);
				printk(".");
				printk(ext);
				printk("   ");
				files_read++;
			}
			file_attrib(&file,&attrib);
			//print_file_attrib(&files[i],&attrib);
		}

	}
	//get_root_files(file_list);
	//list_files(file_list);
	//getdents
	return files_read;
}



int list_files(Fat16Entry *files) {
	int i;
	fileAttrib attrib;
	char name[9];
	char ext[4];

	for(i=0; i < bs.root_dir_entries;i++) {
		if (get_entry_type(&files[i]) == TYPE_FILE) {
			//printf("\nentry file #%d: ",i + 1);
			//printk("\r\nentry file #xxx: ");
			//deleted file
			if (files[i].filename[0] == 0xe5) {
				//printf("deleted (%s)\n",files[i].filename);
				printk("deleted (");
				extract_file_name(&files[i],name,ext);
				printk(name);
				printk(".");
				printk(ext);
				printk(")   ");
				continue;
			}
			//no file
			else if (files[i].filename[0] == 0x00) {
				//printf("no file\n");
				break;
			}
			else {
				//printf("%s\n",files[i].filename);
				extract_file_name(&files[i],name,ext);
				printk(name);
				printk(".");
				printk(ext);
				printk("   ");
			}
			file_attrib(&files[i],&attrib);
			//print_file_attrib(&files[i],&attrib);
		}
	}
	printk("\r\n");
	return 0;
}


int print_file(Fat16Entry *file) {
	unsigned int  curr_cluster;
	unsigned int  next_cluster;
    char clust_buf[4097];
    char nul = '\0';

	//reading file content
	curr_cluster=file->starting_cluster;
	do {
		//DEBUG
//		printk("\r\ncurrent cluster: ");
//		printk(myitoa(curr_cluster));
//		printk("\r\n");
		//END DEBUG
		read_cluster(clust_buf,curr_cluster);
		clust_buf[4096] = '\0';
		//FILL IN!
		//printf("%.4096s",filecontent);
		printk(clust_buf);
		get_next_cluster(curr_cluster,&next_cluster);
		curr_cluster = next_cluster;

		//TEMP!!!!!!
		//curr_cluster = 0x0FF8;
	} while (curr_cluster < 0x0FF8); //cluster value 0FF8h to 0FFFh is end of file
	//FILL IN!
	printf("\r\nEOF\r\n");
	return 0;
}

int load_file_content(Fat16Entry *file,unsigned int segment,unsigned int offset) {
	unsigned int  curr_cluster;
	unsigned int  next_cluster;
    char clust_buf[4096];
    char nul = '\0';

	//reading file content
	curr_cluster=file->starting_cluster;
	do {
		//DEBUG
		//printk("\r\ncurrent cluster: ");
		//printk(myitoa(curr_cluster));
		//printk("\r\n");
		//END DEBUG
		read_cluster(clust_buf,curr_cluster);
		//TODO: load the cluster into segment
		load_to_memory(segment,offset,clust_buf,4096);
		offset += sizeof(clust_buf);

		get_next_cluster(curr_cluster,&next_cluster);
		curr_cluster = next_cluster;

		//TEMP!!!!!!
		//curr_cluster = 0x0FF8;
	} while (curr_cluster < 0x0FF8); //cluster value 0FF8h to 0FFFh is end of file
	//FILL IN!
	//printf("\r\nEOF\r\n");
	return 0;
}


int extract_file_name(Fat16Entry *file,char *filename,char *ext) {
	int i;

	strncpy(filename,file->filename,8);
	filename[8] = '\0';
	strncpy(ext,file->ext,3);
	ext[3] = '\0';

	for (i=0; i < 8;i++) {
		if (filename[i] == ' ') {
			filename[i] = '\0';
		}
	}

	for (i=0; i < 3;i++) {
			if (ext[i] == ' ') {
				ext[i] = '\0';
			}
		}

	return 0;
}

entry_type get_entry_type(Fat16Entry *entry) {
	fileAttrib attrib;

	file_attrib(entry,&attrib);

	//deleted file
	if(entry->filename[0] == 0xe5) {
		return TYPE_DELETED;
	}
	//end of root directory files
	else if (entry->filename[0] == 0x00) {
		return TYPE_NONE;
	}
	//VFAT ENTRY
	else if (entry->starting_cluster == 0 && entry->reserved[0] == 0 &&
			(entry->attributes == 0x0f) && ((entry->filename[0] & 0x41) == 0x41)) {
		return TYPE_VFAT;
	}
	//note that value 0 in FAT entry is empty cluster
	else if(attrib.volume_label) {
		return TYPE_VOLLABEL;
	}
	else {
		return TYPE_FILE;
	}
}


int get_file(Fat16Entry *file_list,char *name,Fat16Entry *file) {
	int i;
	char filename[9];
	char ext[4];


	for (i=0;i < bs.root_dir_entries;i++) {

		if (get_entry_type(&file_list[i]) == TYPE_FILE) {

			extract_file_name(&file_list[i],filename,ext);

			//FILL IN!
			if (strcmp(filename,name) == 0) {
				memcpy(file,&file_list[i],sizeof(Fat16Entry));
				//file->attributes = file_list[i].attributes;
				//strncpy(file->ext,file_list[i].ext,sizeof(file->ext));
				//file->file_size = file_list[i].file_size;
				//strncpy(file->filename,file_list[i].filename,sizeof(file->filename));
				//file->modify_date = file_list[i].modify_date;
				//file->modify_time = file_list[i].modify_time;
				//strncpy(file->reserved,file_list[i].reserved,sizeof(file->reserved));
				//file->starting_cluster = file_list[i].starting_cluster;
				return 0;
			}
		}

    }

	return -1;
}

int get_root_size() {
	return s_pos.data_section_start - s_pos.root_directory_start;
}

int get_root_offset() {
	return s_pos.root_directory_start;
}
