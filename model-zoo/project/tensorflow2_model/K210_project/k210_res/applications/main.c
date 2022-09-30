/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2018/09/30     Bernard      The first version
 */

#include <rtthread.h>
#include <stdio.h>
extern int32_t enter_python(uint32_t delay_s);
int main(void)
{
    rt_kprintf("Hello, world\n");

    /* enter msh or python, default python if enable mpy, type msh to msh */
    enter_python(3);
    return 0;
}

#include <lcd.h>

/*Camera閲囬泦瀹屾垚鍥炶皟鍑芥暟*/
volatile uint32_t g_dvp_finish_flag;
rt_err_t camera_cb(rt_device_t dev, size_t size){
    g_dvp_finish_flag = 1;
    return RT_EOK;
}
int camera_sample(void)
{

#if 1
   rt_kprintf("Hello, world\n");
   /* read涓�甯у浘鍍�,鏄剧ず涓嶢I鍥惧儚浼氭寜椤哄簭杩炵画鎺掓斁鍦╞uffer涓� */
   uint8_t* display_image = rt_malloc((240 * 320 * 2) + (240 * 320 * 3));
    /* LCD init */
   rt_kprintf("LCD init\n");
   lcd_init();
   //娓呭睆
   lcd_clear(BLUE);
   lcd_draw_rectangle(10, 10, 100, 100,1,RED);
   /* DVP init & 鎽勫儚澶村垵濮嬪寲*/
   rt_kprintf("DVP init\n");
   rt_device_t camera_dev = rt_device_find("gc0308"); //鏌ユ壘鎽勫儚澶磋澶�"gc0308"
   if(!camera_dev) {
       rt_kprintf("find camera err!\n");
       return -1;
   };
   rt_device_init(camera_dev); //鍒濆鍖栨憚鍍忓ご
   rt_device_open(camera_dev,RT_DEVICE_OFLAG_RDWR); //鎵撳紑鎽勫儚澶�,璇诲啓妯″紡
   rt_device_set_rx_indicate(camera_dev,camera_cb); //璁剧疆read鍥炶皟鍑芥暟

   /* enable global interrupt ,浣胯兘鍏ㄥ眬涓柇*/
   sysctl_enable_irq();
   while (1){
           g_dvp_finish_flag = 0;
           /* 閲囬泦鍥惧儚鏄剧ず&AI鐢眃isplay_image鍦板潃寮�濮嬭繛缁瓨鏀� */
           rt_device_read(camera_dev,0,display_image,0);
           while (g_dvp_finish_flag == 0) {};  //绛夊緟
           rt_kprintf("dvp cap\n");
//           __pixel_reversal(display_image, 320, 240);//lcd鍍忕礌浜ら敊
           //LCD鏄剧ず
           lcd_draw_picture(0, 0, 320, 240, display_image);
           lcd_draw_rectangle(10, 10, 100, 100,1,RED);
   }
#endif
    return 0;
}
MSH_CMD_EXPORT(camera_sample, camera sample);
