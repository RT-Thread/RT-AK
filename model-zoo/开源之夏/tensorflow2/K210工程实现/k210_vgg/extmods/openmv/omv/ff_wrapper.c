/*
 * This file is part of the OpenMV project.
 * Copyright (c) 2013-2016 Kwabena W. Agyeman <kwagyeman@openmv.io>
 * This work is licensed under the MIT license, see the file LICENSE for details.
 *
 * File System Helper Functions
 *
 */
#include <mp.h>
#include "common.h"
#include "fb_alloc.h"
#include "ff_wrapper.h"

const char *ffs_strerror(int res)
{
    return "";
}

int f_open_helper(omvhal_fil_t *fp, const char *path, int mode)
{
    return omvhal_f_open(fp, path, mode);
}

NORETURN static void ff_fail(omvhal_fil_t *fp, int res)
{
    if (fp) omvhal_f_close(fp);
    nlr_raise(mp_obj_new_exception_msg(&mp_type_OSError, ffs_strerror(res)));
}

NORETURN static void ff_read_fail(omvhal_fil_t *fp)
{
    if (fp) omvhal_f_close(fp);
    nlr_raise(mp_obj_new_exception_msg(&mp_type_OSError, "Failed to read requested bytes!"));
}

NORETURN static void ff_write_fail(omvhal_fil_t *fp)
{
    if (fp) omvhal_f_close(fp);
    nlr_raise(mp_obj_new_exception_msg(&mp_type_OSError, "Failed to write requested bytes!"));
}

NORETURN static void ff_expect_fail(omvhal_fil_t *fp)
{
    if (fp) omvhal_f_close(fp);
    nlr_raise(mp_obj_new_exception_msg(&mp_type_OSError, "Unexpected value read!"));
}

void ff_unsupported_format(omvhal_fil_t*fp)
{
    if (fp) omvhal_f_close(fp);
    nlr_raise(mp_obj_new_exception_msg(&mp_type_OSError, "Unsupported format!"));
}

void ff_file_corrupted(omvhal_fil_t*fp)
{
    if (fp) omvhal_f_close(fp);
    nlr_raise(mp_obj_new_exception_msg(&mp_type_OSError, "File corrupted!"));
}

void ff_not_equal(omvhal_fil_t*fp)
{
    if (fp) omvhal_f_close(fp);
    nlr_raise(mp_obj_new_exception_msg(&mp_type_OSError, "Images not equal!"));
}

void ff_no_intersection(omvhal_fil_t*fp)
{
    if (fp) omvhal_f_close(fp);
    nlr_raise(mp_obj_new_exception_msg(&mp_type_OSError, "No intersection!"));
}

void file_read_open(omvhal_fil_t*fp, const char *path)
{
    int res = f_open_helper(fp, path, FA_READ|FA_OPEN_EXISTING);
    if (res != 0) ff_fail(fp, res);
}

void file_write_open(omvhal_fil_t*fp, const char *path)
{
    int res = f_open_helper(fp, path, FA_WRITE|FA_CREATE_ALWAYS);
    if (res != 0) ff_fail(fp, res);
}

void file_close(omvhal_fil_t*fp)
{
    int res = omvhal_f_close(fp);
    if (res != FR_OK) ff_fail(fp, res);
}

void file_seek(omvhal_fil_t*fp, uint32_t offset)
{
    int res = omvhal_f_lseek(fp, offset);
    if (res != FR_OK) ff_fail(fp, res);
}

// When a sector boundary is encountered while writing a file and there are
// more than 512 bytes left to write FatFs will detect that it can bypass
// its internal write buffer and pass the data buffer passed to it directly
// to the disk write function. However, the disk write function needs the
// buffer to be aligned to a 4-byte boundary. FatFs doesn't know this and
// will pass an unaligned buffer if we don't fix the issue. To fix this problem
// we use a temporary buffer to fix the alignment and to speed everything up.

// We use this temporary buffer for both reads and writes. The buffer allows us
// to do multi-block reads and writes which signifcantly speed things up.

static uint32_t file_buffer_offset = 0;
static uint8_t *file_buffer_pointer = 0;
static uint32_t file_buffer_size = 0;
static uint32_t file_buffer_index = 0;

void file_buffer_init0()
{
    file_buffer_offset = 0;
    file_buffer_pointer = 0;
    file_buffer_size = 0;
    file_buffer_index = 0;
}

static void file_fill(omvhal_fil_t *fp)
{

}

static void file_flush(omvhal_fil_t *fp)
{

}

uint32_t file_tell_w_buf(omvhal_fil_t *fp)
{

}

uint32_t file_size_w_buf(omvhal_fil_t *fp)
{

}

void file_buffer_on(omvhal_fil_t *fp)
{

}

void file_buffer_off(omvhal_fil_t *fp)
{

}

void read_byte(omvhal_fil_t *fp, uint8_t *value)
{
    uint32_t bytes;
    int res = omvhal_f_read(fp, value, sizeof(*value), &bytes);
    if (res != FR_OK) ff_fail(fp, res);
    if (bytes != sizeof(*value)) ff_read_fail(fp);
}

void read_byte_expect(omvhal_fil_t *fp, uint8_t value)
{
    uint8_t compare;
    read_byte(fp, &compare);
    if (value != compare) ff_expect_fail(fp);
}

void read_byte_ignore(omvhal_fil_t *fp)
{
    uint8_t trash;
    read_byte(fp, &trash);
}

void read_word(omvhal_fil_t *fp, uint16_t *value)
{
    uint32_t bytes;
    int res = omvhal_f_read(fp, value, sizeof(*value), &bytes);
    if (res != FR_OK) ff_fail(fp, res);
    if (bytes != sizeof(*value)) ff_read_fail(fp);
}

void read_word_expect(omvhal_fil_t *fp, uint16_t value)
{
    uint16_t compare;
    read_word(fp, &compare);
    if (value != compare) ff_expect_fail(fp);
}

void read_word_ignore(omvhal_fil_t *fp)
{
    uint16_t trash;
    read_word(fp, &trash);
}

void read_long(omvhal_fil_t *fp, uint32_t *value)
{
    uint32_t bytes;
    int res = omvhal_f_read(fp, value, sizeof(*value), &bytes);
    if (res != FR_OK) ff_fail(fp, res);
    if (bytes != sizeof(*value)) ff_read_fail(fp);
}

void read_long_expect(omvhal_fil_t *fp, uint32_t value)
{
    uint32_t compare;
    read_long(fp, &compare);
    if (value != compare) ff_expect_fail(fp);
}

void read_long_ignore(omvhal_fil_t *fp)
{
    uint32_t trash;
    read_long(fp, &trash);
}

void read_data(omvhal_fil_t *fp, void *data, uint32_t size)
{
    uint32_t bytes;
    int res = omvhal_f_read(fp, data, size, &bytes);
    if (res != FR_OK) ff_fail(fp, res);
    if (bytes != size) ff_read_fail(fp);
}

void write_byte(omvhal_fil_t *fp, uint8_t value)
{
    uint32_t bytes;
    int res = omvhal_f_write(fp, &value, sizeof(value), &bytes);
    if (res != FR_OK) ff_fail(fp, res);
    if (bytes != sizeof(value)) ff_write_fail(fp);
}

void write_word(omvhal_fil_t *fp, uint16_t value)
{
    uint32_t bytes;
    int res = omvhal_f_write(fp, &value, sizeof(value), &bytes);
    if (res != FR_OK) ff_fail(fp, res);
    if (bytes != sizeof(value)) ff_write_fail(fp);
}

void write_long(omvhal_fil_t *fp, uint32_t value)
{
    uint32_t bytes;
    int res = omvhal_f_write(fp, &value, sizeof(value), &bytes);
    if (res != FR_OK) ff_fail(fp, res);
    if (bytes != sizeof(value)) ff_write_fail(fp);
}

void write_data(omvhal_fil_t *fp, const void *data, uint32_t size)
{
    uint32_t bytes;
    int res = omvhal_f_write(fp, data, size, &bytes);
    if (res != FR_OK) ff_fail(fp, res);
    if (bytes != size) ff_write_fail(fp);
}
