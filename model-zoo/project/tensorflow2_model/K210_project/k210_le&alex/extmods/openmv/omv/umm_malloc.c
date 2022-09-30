/*
 * This file is part of the OpenMV project.
 *
 * Copyright (c) 2013-2019 Ibrahim Abdelkader <iabdalkader@openmv.io>
 * Copyright (c) 2013-2019 Kwabena W. Agyeman <kwagyeman@openmv.io>
 *
 * This work is licensed under the MIT license, see the file LICENSE for details.
 *
 * UMM memory allocator.
 */
#include <mp.h>
#include <string.h>
#include "fb_alloc.h"
#include "umm_malloc.h"
#include "omv_boardconfig.h"

NORETURN  void umm_alloc_fail()
{
    nlr_raise(mp_obj_new_exception_msg(&mp_type_MemoryError,
        "Out of temporary Frame Buffer Heap Memory!"
        " Please reduce the resolution of the image you are running this algorithm on to bypass this issue!"));
}

/* ------------------------------------------------------------------------- */

void umm_init_x( size_t size ) {
    (void)(size);
}

void umm_init( void ) {
    umm_init_x(0);
}

/* ------------------------------------------------------------------------ */

void umm_free( void *ptr ) {
    free(ptr);
}

/* ------------------------------------------------------------------------ */

void *umm_malloc( size_t size ) {
    return malloc(size);
}

/* ------------------------------------------------------------------------ */

void *umm_realloc( void *ptr, size_t size ) {
    return realloc(ptr, size);
}

/* ------------------------------------------------------------------------ */

void *umm_calloc( size_t num, size_t item_size ) {
    return calloc(num, item_size);
}

/* ------------------------------------------------------------------------ */

////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////
