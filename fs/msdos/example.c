#include "mystdio.h"
#include "fs/filelib.h"

FILE *drv;

int media_init()
{
    drv = fopen("/root/fat.bin","ab+");
	
    return 1;
}

int media_read(unsigned long sector, unsigned char *buffer, unsigned long sector_count)
{
    const long SECTOR_SIZE = 512;
	unsigned long i;

    fseek(drv,SECTOR_SIZE*sector,SEEK_SET);
	fread(buffer,SECTOR_SIZE,sector_count,drv);
 
    return 1;
}

int media_write(unsigned long sector, unsigned char *buffer, unsigned long sector_count)
{
    const long SECTOR_SIZE = 512;
	unsigned long i;

	fseek(drv,SECTOR_SIZE*sector,SEEK_SET);
	fwrite(buffer,SECTOR_SIZE,sector_count,drv);

    return 1;
}

void main()
{
    FL_FILE *file;

    // Initialise media
    media_init();

    // Initialise File IO Library
    fl_init();

    // Attach media access functions to library
    if (fl_attach_media(media_read, media_write) != FAT_INIT_OK)
    {
        printf("ERROR: Media attach failed\n");
        return; 
    }

    // List root directory
    fl_listdirectory("/");

    // Create File
    file = fl_fopen("/file.bin", "w");
    if (file)
    {
        // Write some data
        unsigned char data[] = { 1, 2, 3, 4 };
        if (fl_fwrite(data, 1, sizeof(data), file) != sizeof(data))
            printf("ERROR: Write file failed\n");
    }
    else
        printf("ERROR: Create file failed\n");

    // Close file
    fl_fclose(file);

    // Delete File
    if (fl_remove("/file.bin") < 0)
        printf("ERROR: Delete file failed\n");

    // List root directory
    fl_listdirectory("/");

    fl_shutdown();
}
