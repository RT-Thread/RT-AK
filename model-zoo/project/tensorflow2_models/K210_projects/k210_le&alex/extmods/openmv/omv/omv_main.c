/*
 * This file is part of the OpenMV project.
 *
 * Copyright (c) 2013-2019 Ibrahim Abdelkader <iabdalkader@openmv.io>
 * Copyright (c) 2013-2019 Kwabena W. Agyeman <kwagyeman@openmv.io>
 *
 * This work is licensed under the MIT license, see the file LICENSE for details.
 *
 * main function.
 */
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <rtthread.h>
#ifdef PKG_USING_OPENMV_CP
#include "dfs_fs.h"
#include <py/compile.h>
#include <py/runtime.h>
#include <py/repl.h>
#include <py/gc.h>
#include <py/mperrno.h>
#include <py/stackctrl.h>
#include <py/frozenmod.h>
#include <lib/mp-readline/readline.h>
#include <lib/utils/pyexec.h>
#include "mpgetcharport.h"
#include "mpputsnport.h"
#include "framebuffer.h"
#include "rt_thread_dbg.h"

#include "omv_boardconfig.h"

#include "usbdbg.h"

volatile uint8_t g_isMainDotPyRunning;

RT_WEAK bool usbdbg_script_ready()
{
	return false;
}

#define THREAD_STACK_NO_SYNC   4096
#define THREAD_STACK_WITH_SYNC 8192

static void *stack_top = RT_NULL;
static char *heap = RT_NULL;

void ide_init()
{
    serial_dbg_deinit();
    serial_dbg_init();
	usbdbg_init0();
}
MSH_CMD_EXPORT(ide_init, ide_init);

void omv_main_thread_entry(void *parameter)
{
	int stack_dummy;
    int stack_size_check;
    stack_top = (void *)&stack_dummy;

		
#ifdef RT_USING_DFS
    //struct dfs_fdtable *fd_table_bak = NULL;
    //mp_sys_resource_bak(&fd_table_bak);
#endif
	
omv_restart:	
	mp_getchar_init();
    mp_putsn_init();
#if defined(MICROPYTHON_USING_FILE_SYNC_VIA_IDE)
    stack_size_check = THREAD_STACK_WITH_SYNC;
#else
    stack_size_check = THREAD_STACK_NO_SYNC;
#endif	
	
	if (rt_thread_self()->stack_size < stack_size_check) 
    {
        mp_printf(&mp_plat_print, "The stack (%.*s) size for executing MicroPython must be >= %d\n", RT_NAME_MAX, rt_thread_self()->name, stack_size_check);
    }
	
#if MICROPY_PY_THREAD
    mp_thread_init(rt_thread_self()->stack_addr, ((rt_uint32_t)stack_top - (rt_uint32_t)rt_thread_self()->stack_addr) / 4);
#endif

    mp_stack_set_top(stack_top);
    // Make MicroPython's stack limit somewhat smaller than full stack available
    mp_stack_set_limit(rt_thread_self()->stack_size - 1024);

#if MICROPY_ENABLE_GC
    heap = rt_malloc(MICROPY_HEAP_SIZE);
    if (!heap) {
        mp_printf(&mp_plat_print, "No memory for MicroPython Heap!\n");
        return;
    }
    gc_init(heap, heap + MICROPY_HEAP_SIZE);
#endif
	
	/* MicroPython initialization */
    mp_init();

    /* system path initialization */
    mp_obj_list_init(mp_sys_path, 0);
    mp_obj_list_append(mp_sys_path, MP_OBJ_NEW_QSTR(MP_QSTR_)); // current dir (or base dir of the script)
    mp_obj_list_append(mp_sys_path, mp_obj_new_str(MICROPY_PY_PATH_FIRST, strlen(MICROPY_PY_PATH_FIRST)));
    mp_obj_list_append(mp_sys_path, mp_obj_new_str(MICROPY_PY_PATH_SECOND, strlen(MICROPY_PY_PATH_SECOND)));
    mp_obj_list_init(mp_sys_argv, 0);
    readline_init0();

	#ifndef OMV_MPY_ONLY
	fb_alloc_init0();
	#endif

    ide_init();
	
	//run cmm_load.py
	mp_import_stat_t stat = mp_import_stat("/cmm_load.py");
    if (stat == MP_IMPORT_STAT_FILE) {
        nlr_buf_t nlr;
		mp_printf(&mp_plat_print, "Found and execute cmm_load.py!\r\n");
        if (nlr_push(&nlr) == 0) {
            int ret = pyexec_file("/cmm_load.py");
            if (ret & PYEXEC_FORCED_EXIT) {
				mp_printf(&mp_plat_print, "cmm_load.py error!\r\n");
                ret = 1;
            }
            if (!ret) {
            }
            nlr_pop();
        }
        else {           
        }
    }
	// rocky: pyb's main uses different method to access file system from omv
	stat = mp_import_stat("/main.py");
	if (stat == MP_IMPORT_STAT_FILE) {
		nlr_buf_t nlr;
		if (nlr_push(&nlr) == 0) {
			g_isMainDotPyRunning = 1;
			mp_printf(&mp_plat_print, "Found and execute main.py!\r\n");
			int ret = pyexec_file("/main.py");
			
			if (ret & PYEXEC_FORCED_EXIT) {
				ret = 1;
			}
			if (!ret) {
				
			}
			nlr_pop();
		}
		else {
			g_isMainDotPyRunning = 0;
			
			#if 0
			// 2019.03.27 19:52 rocky: if main.py is interrupted by running another script, 
			// we have to do soft reset, otherwise fb alloc logic may fail and led to hard fault
			// In this case, it makes user have to press start button twice to start the script in OpenMV IDE
			goto cleanup;
			#else
			fb_free_all();
			#endif                    
		}
		
		mp_printf(&mp_plat_print, "Exit from main.py!\r\n");
	}
	g_isMainDotPyRunning = 0;
RunREPL:
#if 1		
    usbdbg_init();
    while (!usbdbg_script_ready()) {
        g_isMainDotPyRunning = 1;
        nlr_buf_t nlr;
        rt_thread_delay(10);
        if (nlr_push(&nlr) == 0) {
            // run REPL
            if (pyexec_mode_kind == PYEXEC_MODE_RAW_REPL) {
                if (pyexec_raw_repl() != 0) {
                    break;
                }
            } else {
                if (pyexec_friendly_repl() != 0) {
					
                    break;
                }
            }

            nlr_pop();
        }
    }
    g_isMainDotPyRunning = 0;
	//mp_printf(&mp_plat_print, "Exit from repy!\r\n");
    if (usbdbg_script_ready()) {
        nlr_buf_t nlr;
		//mp_printf(&mp_plat_print, "script ready!\r\n");
        // execute the script
        if (nlr_push(&nlr) == 0) {
	
			// rocky: 2019.03.27 19:00 reset fb alloc memory for new script
			#ifndef OMV_MPY_ONLY
			fb_alloc_init0();
			#endif
#if 0
			vstr_t *buf = usbdbg_get_script();
			mp_obj_t code = pyexec_compile_str(buf);	
            // enable IDE interrupt
            usbdbg_set_irq_enabled(true);
            pyexec_exec_code(code);
#else
			usbdbg_set_irq_enabled(true);
            //rt_thread_delay(10);
			pyexec_str((vstr_t *)usbdbg_get_script());
#endif
        } else {
            mp_obj_print_exception(&mp_plat_print, (mp_obj_t)nlr.ret_val);
            usbdbg_stop_script();
            goto RunREPL;   // rocky: script is stopped, waiting for next script
        }

    }
	goto RunREPL;
#endif	

	
	gc_sweep_all();

    mp_deinit();

#if MICROPY_PY_THREAD
    mp_thread_deinit();
#endif

    rt_free(heap);

    mp_putsn_deinit();
    mp_getchar_deinit();
    
#ifdef RT_USING_DFS
    //mp_sys_resource_gc(fd_table_bak);
#endif

	rt_thread_mdelay(500);
	goto omv_restart;
}
rt_thread_t main_thread;
void omv_main()
{

    main_thread = rt_thread_create("omv_main", omv_main_thread_entry, RT_NULL, 0x8000, RT_MAIN_THREAD_PRIORITY-5, 20);

	rt_thread_startup(main_thread);
}

static void omv(uint8_t argc, char **argv) {
    omv_main();
}
//INIT_APP_EXPORT(omv_main);
MSH_CMD_EXPORT(omv, OpenMV: `execute python script);
#endif /* PKG_USING_OPENMV_CP */
