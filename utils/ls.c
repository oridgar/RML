#include <stdio.h>
#include <io.h>
#include <sysdef.h>
#include <stdlib.h>

void main() {
	int i,j;
	char buf[512];
	int files;
	file_out_format out_format;
	unsigned long total =0;
	SYSCALL_FILE_ENTRY *curr_entry;

	files = list_root_files(buf);

	for (i=0; i < files; i++) {
		curr_entry = &((SYSCALL_FILE_ENTRY *)buf)[i];

		switch (out_format) {
		case FILE_OUT_WIDE:
			printf(curr_entry->name);
			printf(".");
			printf(curr_entry->ext);
			printf(" ");
			printf("   ");
			break;
		case FILE_OUT_LIST:
			if (curr_entry->dir) {
				printf("d");
			}
			else {
				printf("-");
			}

			for(j=0; j < 3; j++) {
				printf("r");
				if (curr_entry->read_only) {
					printf("-");
				}
				else {
					printf("w");
				}
				printf("x");
			}

			printf(" root root ");
			printf(ultoa(curr_entry->size));
			printf(" ");

			switch ((unsigned int)curr_entry->month) {
				case 1:
					printf("Jan");
					break;
				case 2:
					printf("Feb");
					break;
				case 3:
					printf("Mar");
					break;
				case 4:
					printf("Apr");
					break;
				case 5:
					printf("May");
					break;
				case 6:
					printf("Jun");
					break;
				case 7:
					printf("Jul");
					break;
				case 8:
					printf("Aug");
					break;
				case 9:
					printf("Sep");
					break;
				case 10:
					printf("Oct");
					break;
				case 11:
					printf("Nov");
					break;
				case 12:
					printf("Dec");
					break;
				default:
					printf("   ");
			}
			printf(" ");
			printf(itoa((unsigned int)curr_entry->day));
			printf(" ");
			printf(itoa((unsigned int)curr_entry->hours));
			printf(":");
			printf(itoa((unsigned int)curr_entry->minutes));

			printf(" ");
			printf(curr_entry->name);
			printf(".");
			printf(curr_entry->ext);
			printf("\r\n");
			break;
		}
		total += (unsigned int)curr_entry->size;
	}
	printf("total ");
	printf(ultoa(total));
	printf("\r\n");


	//return 0;
}
