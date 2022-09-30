/*
 * This file is part of the OpenMV project.
 * Copyright (c) 2013-2016 Kwabena W. Agyeman <kwagyeman@openmv.io>
 * This work is licensed under the MIT license, see the file LICENSE for details.
 *
 * File System Helper Functions
 *
 */
#ifndef __FF_WRAPPER_H__
#define __FF_WRAPPER_H__

#include "omvhalfile.h"

#define FA_WRITE         0x01
#define FA_CREATE_ALWAYS 0x02
#define FA_READ          0x04
#define FA_OPEN_EXISTING 0x08

const char *ffs_strerror(int res);

int f_open_helper(omvhal_fil_t *fp, const char *path, int mode);

void ff_unsupported_format(omvhal_fil_t*fp);
void ff_file_corrupted(omvhal_fil_t*fp);
void ff_not_equal(omvhal_fil_t*fp);
void ff_no_intersection(omvhal_fil_t*fp);
void file_read_open(omvhal_fil_t*fp, const char *path);
void file_write_open(omvhal_fil_t*fp, const char *path);
void file_close(omvhal_fil_t*fp);
void file_seek(omvhal_fil_t*fp, uint32_t offset);
void file_truncate(omvhal_fil_t*fp);
void file_sync(omvhal_fil_t*fp);

// File buffer functions.
void file_buffer_init0();
void file_buffer_on(omvhal_fil_t*fp); // does fb_alloc_all
uint32_t file_tell_w_buf(omvhal_fil_t*fp); // valid between on and off
uint32_t file_size_w_buf(omvhal_fil_t*fp); // valid between on and off
void file_buffer_off(omvhal_fil_t*fp); // does fb_free
void read_byte(omvhal_fil_t*fp, uint8_t *value);
void read_byte_expect(omvhal_fil_t*fp, uint8_t value);
void read_byte_ignore(omvhal_fil_t*fp);
void read_word(omvhal_fil_t*fp, uint16_t *value);
void read_word_expect(omvhal_fil_t*fp, uint16_t value);
void read_word_ignore(omvhal_fil_t*fp);
void read_long(omvhal_fil_t*fp, uint32_t *value);
void read_long_expect(omvhal_fil_t*fp, uint32_t value);
void read_long_ignore(omvhal_fil_t*fp);
void read_data(omvhal_fil_t*fp, void *data, uint32_t size);
void write_byte(omvhal_fil_t*fp, uint8_t value);
void write_word(omvhal_fil_t*fp, uint16_t value);
void write_long(omvhal_fil_t*fp, uint32_t value);
void write_data(omvhal_fil_t*fp, const void *data, uint32_t size);

#endif /* __FF_WRAPPER_H__ */
