#ifndef __OMVHALFILE_H
#define __OMVHALFILE_H

#include "omvhaldef.h"

#define FIL omvhal_fil_t
#define FRESULT int
#define UINT uint32_t
#define FR_OK 0

#define f_tell omvhal_f_tell
#define f_size omvhal_f_size
#define f_write omvhal_f_write
#define f_read omvhal_f_read
#define f_close omvhal_f_close
#define f_eof omvhal_f_eof

int omvhal_f_open(omvhal_fil_t *f, const char *path, int mode);
int omvhal_f_close(omvhal_fil_t *f);
int omvhal_f_write(omvhal_fil_t *fp, const void* buff, uint32_t btw, uint32_t* bw);
int omvhal_f_read(omvhal_fil_t* fp, void* buff, uint32_t btr, uint32_t* br);
int omvhal_f_eof(omvhal_fil_t* fp);
uint32_t omvhal_f_size(omvhal_fil_t* fp);
uint32_t omvhal_f_tell(omvhal_fil_t* fp);
int omvhal_f_lseek(omvhal_fil_t*fp, uint32_t offset);

#endif
