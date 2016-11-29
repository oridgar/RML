#ifndef __FAT_FORMAT_H__
#define __FAT_FORMAT_H__

#include "fs/defs.h"
#include "fs/opts.h"
#include "fs/access.h"

//-----------------------------------------------------------------------------
// Prototypes
//-----------------------------------------------------------------------------
int fatfs_format(struct fatfs *fs, uint32 volume_sectors, const char *name);
int fatfs_format_fat16(struct fatfs *fs, uint32 volume_sectors, const char *name);
int fatfs_format_fat32(struct fatfs *fs, uint32 volume_sectors, const char *name);

#endif
