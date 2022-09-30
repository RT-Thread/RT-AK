#ifndef __OMVHALDEF_H
#define __OMVHALDEF_H

#include <stdint.h>

typedef struct
{
    unsigned char reg_len;
    int pixformat;
    uint32_t width;
    uint32_t height;
} omvhal_cambus_t;

typedef struct
{
    int lock;
    uint32_t tid;
} omvhal_mutex_t;

typedef struct
{
    void *dev;
    unsigned char *fb;
    unsigned short width, height;
} ovmhal_lcd_t;

typedef struct
{
    int fd;
} omvhal_fil_t;

#endif
