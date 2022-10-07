/*
 * This file is part of the OpenMV project.
 * Copyright (c) 2013-2016 Kwabena W. Agyeman <kwagyeman@openmv.io>
 * This work is licensed under the MIT license, see the file LICENSE for details.
 *
 * Interface for using extra frame buffer RAM as a stack.
 *
 */
#include <mp.h>
#include "fb_alloc.h"
#include "framebuffer.h"
#include "omv_boardconfig.h"

#define FB_MAX_ALLOC_TIMES 12

typedef struct
{
    uint16_t valid;
    uint16_t count;
    uint16_t mark;
    uint16_t permanent;
    void *p;
} fb_alloc_addr_info_t;

static fb_alloc_addr_info_t m_fb_alloc_addr[FB_MAX_ALLOC_TIMES] = {0}; //must <255
static uint16_t m_count_max_now = 0;
static uint16_t m_mark_max_now = 0;

NORETURN void fb_alloc_fail()
{
    nlr_raise(mp_obj_new_exception_msg(&mp_type_MemoryError,
                                       "Out of Memory!"
                                       " Please reduce the resolution of the image you are running this algorithm on to bypass this issue!"));
}

NORETURN void fb_alloc_fail_2()
{
    nlr_raise(mp_obj_new_exception_msg(&mp_type_MemoryError,
                                       "Too many fb_alloc! no space save!"
                                       " try again or reduce img size!"));
}

void fb_alloc_init0()
{
    memset(m_fb_alloc_addr, 0, sizeof(m_fb_alloc_addr));
    m_count_max_now = 0;
    m_mark_max_now = 0;
}

uint32_t fb_avail()
{
    return OMV_FB_ALLOC_SIZE; //TODO
}

void fb_alloc_mark()
{
    ++m_mark_max_now;
}

static void int_fb_alloc_free_till_mark(bool free_permanent)
{
    uint8_t i;

    for (i = 0; i < FB_MAX_ALLOC_TIMES; ++i)
    {
        if (m_fb_alloc_addr[i].valid && m_fb_alloc_addr[i].mark == m_mark_max_now)
        {
            --m_count_max_now;
            if ((!free_permanent) && (m_fb_alloc_addr[i].permanent))
                continue;

            free(m_fb_alloc_addr[i].p);
            m_fb_alloc_addr[i].p = NULL;
            m_fb_alloc_addr[i].valid = false;
            m_fb_alloc_addr[i].permanent = 0;
        }
    }
    --m_mark_max_now;
}

void fb_alloc_free_till_mark()
{
    int_fb_alloc_free_till_mark(false);
}

void *fb_alloc(uint32_t size, int hints)
{
    uint8_t i;
    void *p;

    if (!size)
    {
        return NULL;
    }

    size=((size+sizeof(uint32_t)-1)/sizeof(uint32_t))*sizeof(uint32_t);// Round Up
    p = malloc(size);
    if (!p)
    {
        mp_printf(&mp_plat_print, "fb alloc %d fail\r\n", size);
        fb_alloc_fail();
    }

    for (i = 0; i < FB_MAX_ALLOC_TIMES; ++i)
    {
        if (!m_fb_alloc_addr[i].valid)
        {
            m_fb_alloc_addr[i].valid = true;
            m_fb_alloc_addr[i].p = p;
            m_fb_alloc_addr[i].mark = m_mark_max_now;
            ++m_count_max_now;
            m_fb_alloc_addr[i].count = m_count_max_now;
            break;
        }
    }
    if (i == FB_MAX_ALLOC_TIMES)
    {
        free(p);
        fb_alloc_fail_2();
    }

    return m_fb_alloc_addr[i].p;
}

// returns null pointer without error if passed size==0
void *fb_alloc0(uint32_t size, int hints)
{
    void *mem = fb_alloc(size, 0);
    memset(mem, 0, size); // does nothing if size is zero.
    return mem;
}

void *fb_alloc_all(uint32_t *size, int hints)
{
    uint8_t i = 0;

    void *p = malloc(OMV_FB_ALLOC_SIZE);
    if (!p)
        fb_alloc_fail();
    for (i = 0; i < FB_MAX_ALLOC_TIMES; ++i)
    {
        if (!m_fb_alloc_addr[i].valid)
        {
            m_fb_alloc_addr[i].valid = true;
            m_fb_alloc_addr[i].p = p;
            m_fb_alloc_addr[i].mark = m_mark_max_now;
            ++m_count_max_now;
            m_fb_alloc_addr[i].count = m_count_max_now;
            break;
        }
    }
    if (i == FB_MAX_ALLOC_TIMES)
    {
        free(p);
        fb_alloc_fail_2();
    }
    *size = OMV_FB_ALLOC_SIZE;

    return m_fb_alloc_addr[i].p;
}

// returns null pointer without error if returned size==0
void *fb_alloc0_all(uint32_t *size, int hints)
{
    void *mem = fb_alloc_all(size, 0);
    memset(mem, 0, *size); // does nothing if size is zero.
    return mem;
}

void fb_free()
{
    uint8_t i;

    for (i = 0; i < FB_MAX_ALLOC_TIMES; ++i)
    {
        if (m_fb_alloc_addr[i].valid && m_fb_alloc_addr[i].count == m_count_max_now)
        {
            free(m_fb_alloc_addr[i].p);
            m_fb_alloc_addr[i].p = NULL;
            m_fb_alloc_addr[i].valid = false;
            --m_count_max_now;
            break;
        }
    }
}

void fb_free_all()
{
    uint8_t i;

    for (i = 0; i < FB_MAX_ALLOC_TIMES; ++i)
    {
        if (m_fb_alloc_addr[i].valid)
        {
            free(m_fb_alloc_addr[i].p);
            m_fb_alloc_addr[i].p = NULL;
            m_fb_alloc_addr[i].valid = false;
        }
    }
    m_count_max_now = 0;
    m_mark_max_now = 0;
}

void fb_alloc_mark_permanent()
{
    int i;

    for (i = 0; i < FB_MAX_ALLOC_TIMES; ++i)
    {
        if (m_fb_alloc_addr[i].mark == m_mark_max_now)
        {
            m_fb_alloc_addr[i].permanent = 1;
        }
    }
}

void fb_alloc_free_till_mark_past_mark_permanent()
{
    int_fb_alloc_free_till_mark(true);
}
