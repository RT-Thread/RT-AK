/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2019-03-19     ZYH          first version
 */

#include <rtthread.h>
#include <fpioa.h>
#include <drv_io_config.h>
#include <sysctl.h>

#define HS_GPIO(n) (FUNC_GPIOHS0 + n)

#define IOCONFIG(pin,func)  {pin, func, #func}

static struct io_config
{
    int io_num;
    fpioa_function_t func;
    const char * func_name;
} io_config[] =
{
#ifdef BSP_USING_LCD
    IOCONFIG(BSP_LCD_CS_PIN, FUNC_SPI0_SS0),                 /* LCD CS PIN */
    IOCONFIG(BSP_LCD_WR_PIN, FUNC_SPI0_SCLK),                /* LCD WR PIN */
    IOCONFIG(BSP_LCD_DC_PIN, HS_GPIO(LCD_DC_PIN)),           /* LCD DC PIN */
#if BSP_LCD_RST_PIN >= 0
    IOCONFIG(BSP_LCD_RST_PIN, HS_GPIO(LCD_RST_PIN)),         /* LCD RESET PIN */
#endif
#if BSP_LCD_BACKLIGHT_PIN >= 0
    IOCONFIG(BSP_LCD_BACKLIGHT_PIN, HS_GPIO(LCD_BACKLIGHT_PIN)),    /* LCD BACKLIGHT PIN */
#endif
#endif

#ifdef BSP_USING_CAMERA
    IOCONFIG(BSP_CAMERA_SCCB_SDA_PIN, FUNC_SCCB_SDA),
    IOCONFIG(BSP_CAMERA_SCCB_SCLK_PIN, FUNC_SCCB_SCLK),
    IOCONFIG(BSP_CAMERA_CMOS_RST_PIN, FUNC_CMOS_RST),
    IOCONFIG(BSP_CAMERA_CMOS_VSYNC_PIN, FUNC_CMOS_VSYNC),
    IOCONFIG(BSP_CAMERA_CMOS_PWDN_PIN, FUNC_CMOS_PWDN),
    IOCONFIG(BSP_CAMERA_CMOS_XCLK_PIN, FUNC_CMOS_XCLK),
    IOCONFIG(BSP_CAMERA_CMOS_PCLK_PIN, FUNC_CMOS_PCLK),
    IOCONFIG(BSP_CAMERA_CMOS_HREF_PIN, FUNC_CMOS_HREF),
#endif

#ifdef BSP_USING_SPI1
    IOCONFIG(BSP_SPI1_CLK_PIN, FUNC_SPI1_SCLK),
    IOCONFIG(BSP_SPI1_D0_PIN, FUNC_SPI1_D0),
    IOCONFIG(BSP_SPI1_D1_PIN, FUNC_SPI1_D1),
#ifdef BSP_USING_SPI1_AS_QSPI
    IOCONFIG(BSP_SPI1_D2_PIN, FUNC_SPI1_D2),
    IOCONFIG(BSP_SPI1_D3_PIN, FUNC_SPI1_D3),
#endif
#ifdef BSP_SPI1_USING_SS0
    IOCONFIG(BSP_SPI1_SS0_PIN, HS_GPIO(SPI1_CS0_PIN)),
#endif
#ifdef BSP_SPI1_USING_SS1
    IOCONFIG(BSP_SPI1_SS1_PIN, HS_GPIO(SPI1_CS1_PIN)),
#endif
#ifdef BSP_SPI1_USING_SS2
    IOCONFIG(BSP_SPI1_SS2_PIN, HS_GPIO(SPI1_CS2_PIN)),
#endif
#ifdef BSP_SPI1_USING_SS3
    IOCONFIG(BSP_SPI1_SS3_PIN, HS_GPIO(SPI1_CS3_PIN)),
#endif
#endif

#ifdef BSP_USING_UART1
    IOCONFIG(BSP_UART1_TXD_PIN, FUNC_UART1_TX),
    IOCONFIG(BSP_UART1_RXD_PIN, FUNC_UART1_RX),
#endif
#ifdef BSP_USING_UART2
    IOCONFIG(BSP_UART2_TXD_PIN, FUNC_UART2_TX),
    IOCONFIG(BSP_UART2_RXD_PIN, FUNC_UART2_RX),
#endif
#ifdef BSP_USING_UART3
    IOCONFIG(BSP_UART3_TXD_PIN, FUNC_UART3_TX),
    IOCONFIG(BSP_UART3_RXD_PIN, FUNC_UART3_RX),
#endif

#ifdef RT_USING_AUDIO
#ifdef BSP_USING_I2S0
    IOCONFIG(BSP_I2S0_OUT_D1_PIN, FUNC_I2S0_OUT_D1),
    IOCONFIG(BSP_I2S0_WS_PIN, FUNC_I2S0_WS),
    IOCONFIG(BSP_I2S0_SCLK_PIN, FUNC_I2S0_SCLK),
#endif
#ifdef BSP_USING_I2S1
    IOCONFIG(BSP_I2S1_IN_D0_PIN, FUNC_I2S1_IN_D0),
    IOCONFIG(BSP_I2S1_WS_PIN, FUNC_I2S1_WS),
    IOCONFIG(BSP_I2S1_SCLK_PIN, FUNC_I2S1_SCLK),
#endif
#endif

#ifdef BSP_USING_I2C0
    IOCONFIG(BSP_I2C0_SCL_PIN, FUNC_I2C0_SCLK),
    IOCONFIG(BSP_I2C0_SDA_PIN, FUNC_I2C0_SDA),
#endif
#ifdef BSP_USING_I2C1
    IOCONFIG(BSP_I2C1_SCL_PIN, FUNC_I2C1_SCLK),
    IOCONFIG(BSP_I2C1_SDA_PIN, FUNC_I2C1_SDA),
#endif
#ifdef BSP_USING_I2C2
    IOCONFIG(BSP_I2C2_SCL_PIN, FUNC_I2C2_SCLK),
    IOCONFIG(BSP_I2C2_SDA_PIN, FUNC_I2C2_SDA),
#endif

#ifdef BSP_PWM_CHN0_ENABLE
    IOCONFIG(BSP_PWM_CHN0_PIN, FUNC_TIMER2_TOGGLE1),
#endif
#ifdef BSP_PWM_CHN1_ENABLE
    IOCONFIG(BSP_PWM_CHN1_PIN, FUNC_TIMER2_TOGGLE2),
#endif
#ifdef BSP_PWM_CHN2_ENABLE
    IOCONFIG(BSP_PWM_CHN2_PIN, FUNC_TIMER2_TOGGLE3),
#endif
#ifdef BSP_PWM_CHN3_ENABLE
    IOCONFIG(BSP_PWM_CHN3_PIN, FUNC_TIMER2_TOGGLE4),
#endif

};



static int print_io_config()
{
    int i;
    rt_kprintf("IO Configuration Table\n");
    rt_kprintf("鈹屸攢鈹�鈹�鈹�鈹�鈹�鈹�鈹攢鈹�鈹�鈹�鈹�鈹�鈹�鈹�鈹�鈹�鈹�鈹�鈹�鈹�鈹�鈹�鈹�鈹�鈹�鈹�鈹�鈹�鈹�鈹�鈹怽n");
    rt_kprintf("鈹侾in    鈹侳unction                鈹俓n");
    rt_kprintf("鈹溾攢鈹�鈹�鈹�鈹�鈹�鈹�鈹尖攢鈹�鈹�鈹�鈹�鈹�鈹�鈹�鈹�鈹�鈹�鈹�鈹�鈹�鈹�鈹�鈹�鈹�鈹�鈹�鈹�鈹�鈹�鈹�鈹n");
    for(i = 0; i < sizeof io_config / sizeof io_config[0]; i++)
    {
        rt_kprintf("鈹�%-2d     鈹�%-24.24s鈹俓n", io_config[i].io_num, io_config[i].func_name);
    }
    rt_kprintf("鈹斺攢鈹�鈹�鈹�鈹�鈹�鈹�鈹粹攢鈹�鈹�鈹�鈹�鈹�鈹�鈹�鈹�鈹�鈹�鈹�鈹�鈹�鈹�鈹�鈹�鈹�鈹�鈹�鈹�鈹�鈹�鈹�鈹榎n");
    return 0;
}
MSH_CMD_EXPORT_ALIAS(print_io_config, io, print io config);

int io_config_init(void)
{
    int count = sizeof(io_config) / sizeof(io_config[0]);
    int i;

/* IO GroupA Power Supply Setting */
#if defined(BSP_GROUPA_POWER_SUPPLY_3V3)
    sysctl_set_power_mode(SYSCTL_POWER_BANK0, SYSCTL_POWER_V33);
    sysctl_set_power_mode(SYSCTL_POWER_BANK1, SYSCTL_POWER_V33);
    sysctl_set_power_mode(SYSCTL_POWER_BANK2, SYSCTL_POWER_V33);
#else
    sysctl_set_power_mode(SYSCTL_POWER_BANK0, SYSCTL_POWER_V18);
    sysctl_set_power_mode(SYSCTL_POWER_BANK1, SYSCTL_POWER_V18);
    sysctl_set_power_mode(SYSCTL_POWER_BANK2, SYSCTL_POWER_V18);
#endif

/* IO GroupB Power Supply Setting */
#if defined(BSP_GROUPB_POWER_SUPPLY_3V3)
    sysctl_set_power_mode(SYSCTL_POWER_BANK3, SYSCTL_POWER_V33);
    sysctl_set_power_mode(SYSCTL_POWER_BANK4, SYSCTL_POWER_V33);
    sysctl_set_power_mode(SYSCTL_POWER_BANK5, SYSCTL_POWER_V33);
#else
    sysctl_set_power_mode(SYSCTL_POWER_BANK3, SYSCTL_POWER_V18);
    sysctl_set_power_mode(SYSCTL_POWER_BANK4, SYSCTL_POWER_V18);
    sysctl_set_power_mode(SYSCTL_POWER_BANK5, SYSCTL_POWER_V18);
#endif

/* IO GroupC Power Supply Setting */
#if defined(BSP_GROUPC_POWER_SUPPLY_3V3)
    sysctl_set_power_mode(SYSCTL_POWER_BANK6, SYSCTL_POWER_V33);
    sysctl_set_power_mode(SYSCTL_POWER_BANK7, SYSCTL_POWER_V33);
#else
    sysctl_set_power_mode(SYSCTL_POWER_BANK6, SYSCTL_POWER_V18);
    sysctl_set_power_mode(SYSCTL_POWER_BANK7, SYSCTL_POWER_V18);
#endif

    for(i = 0; i < count; i++)
    {
        fpioa_set_function(io_config[i].io_num, io_config[i].func);
    }

#if defined(BSP_USING_CAMERA) || defined(BSP_USING_LCD)
    sysctl_set_spi0_dvp_data(1);
#endif

}
INIT_BOARD_EXPORT(io_config_init);

int io_config_used(int io_num)
{
    int count = sizeof(io_config) / sizeof(io_config[0]);
    int i;

    for(i = 0; i < count; i++)
    {
        if (io_config[i].io_num == io_num)
            break;
    }

    return (i < count);
}
