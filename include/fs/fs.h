#ifndef _FS_H
#define _FS_H

#include <sysdef.h>
//----------------
//types definition
//----------------

typedef enum {TYPE_VFAT,TYPE_DELETED,TYPE_NONE,TYPE_VOLLABEL,TYPE_FILE} entry_type;

typedef struct __attribute__((__packed__)) {
    unsigned char jmp[3];
    char oem[8];
    unsigned short sector_size;
    unsigned char sectors_per_cluster;
    unsigned short reserved_sectors;
    unsigned char number_of_fats;
    unsigned short root_dir_entries;
    unsigned short total_sectors_short; // if zero, later field is used
    unsigned char media_descriptor;
    unsigned short fat_size_sectors;
    unsigned short sectors_per_track;
    unsigned short number_of_heads;
    unsigned int hidden_sectors;
    unsigned int total_sectors_long;
    //extended BIOS Parameter Block for FAT 12 and FAT 16
    unsigned char drive_number;
    unsigned char current_head;
    unsigned char boot_signature;
    unsigned int volume_id;
    char volume_label[11];
    char fs_type[8];
    char boot_code[448];
    unsigned short boot_sector_signature;
}  Fat16BootSector;

typedef struct __attribute__((__packed__)) {
	unsigned char filename[8];
    unsigned char ext[3];
    unsigned char attributes;
    unsigned char reserved[10];
    unsigned short modify_time;
    unsigned short modify_date;
    unsigned short starting_cluster;
    unsigned long   file_size;
} Fat16Entry;

typedef struct  __attribute__((__packed__)) {
	unsigned int fat_section_start;
	unsigned int  data_section_start;
	unsigned int root_directory_start;
} sectionPositions;

typedef struct __attribute__((__packed__)) {
	char read_only;
	char hidden;
	char system_file;
	char volume_label;
	char subdirectory;
	char archive;
} fileAttrib;

entry_type get_entry_type(Fat16Entry *entry);
int open_drive();
int read_boot_sector();
void print_fs_params();
int read_fat(char* fatcache);
int read_root_dir(char *rootcache);
int read_cluster(char *buffer,unsigned int cluster_num);
int file_attrib(Fat16Entry *file,fileAttrib *attrib);
int get_next_cluster(unsigned int curr_cluster,unsigned int *next_cluster);
int get_root_files(Fat16Entry *files);
int print_file(Fat16Entry *file);
int load_file_content(Fat16Entry *file,unsigned int segment,unsigned int offset);
int extract_file_name(Fat16Entry *file,char *filename,char *ext);
int get_file(Fat16Entry *file_list,char *name,Fat16Entry *file);
int get_root_size();
int get_root_offset();
int list_root_files(unsigned int buf_size, SYSCALL_FILE_ENTRY *buf, file_out_format out_format);


#endif
