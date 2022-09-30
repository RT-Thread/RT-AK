#include <project_0/driver/lcd_bare/board_config.h>

void io_mux_init(void) {
#if BOARD_LICHEEDAN || YB_DKA01
    fpioa_set_function(LCD_PIN_RST, FUNC_LCD_RST);
    // sysctl_set_spi0_dvp_data(1);
#endif
    /* Init DVP IO map and function settings */
    fpioa_set_function(DVP_PIN_RST, FUNC_CMOS_RST);
    fpioa_set_function(DVP_PIN_PWDN, FUNC_CMOS_PWDN);
    fpioa_set_function(DVP_PIN_XCLK, FUNC_CMOS_XCLK);
    fpioa_set_function(DVP_PIN_VSYNC, FUNC_CMOS_VSYNC);
    fpioa_set_function(DVP_PIN_HSYNC, FUNC_CMOS_HREF);
    fpioa_set_function(DVP_PIN_PCLK, FUNC_CMOS_PCLK);
    fpioa_set_function(DVP_PIN_SCL, FUNC_SCCB_SCLK);
    fpioa_set_function(DVP_PIN_SDA, FUNC_SCCB_SDA);
#ifndef BSP_USING_LCD
    /* Init SPI IO map and function settings */
    fpioa_set_function(LCD_PIN_RS, FUNC_LCD_RS);
    fpioa_set_function(LCD_PIN_CS, FUNC_LCD_CS);
    fpioa_set_function(LCD_PIN_WR, FUNC_LCD_WR);
    #ifdef LCD_PIN_BACKLIGHT
    fpioa_set_function(LCD_PIN_BACKLIGHT, FUNC_LCD_BACKLIGHT);
    gpiohs_set_drive_mode(LCD_BACKLIGHT_GPIONUM, GPIO_DM_OUTPUT);
    gpiohs_set_pin(LCD_BACKLIGHT_GPIONUM, LCD_BACKLIGHT_ACTIVE);
    #endif
    // 使能SPI0和DVP
    sysctl_set_spi0_dvp_data(1);
#endif
// #endif
}

void io_set_power(void) {
#if BOARD_LICHEEDAN || YB_DKA01
    /* Set dvp and spi pin to 1.8V */
    sysctl_set_power_mode(SYSCTL_POWER_BANK6, SYSCTL_POWER_V18);
    sysctl_set_power_mode(SYSCTL_POWER_BANK7, SYSCTL_POWER_V18);
#elif BOARD_KD233
    /* Set dvp and spi pin to 1.8V */
    sysctl_set_power_mode(SYSCTL_POWER_BANK0, SYSCTL_POWER_V18);
    sysctl_set_power_mode(SYSCTL_POWER_BANK1, SYSCTL_POWER_V18);
    sysctl_set_power_mode(SYSCTL_POWER_BANK2, SYSCTL_POWER_V18);
#elif EDU_KIT
    /* Set dvp and spi pin to 1.8V */
    sysctl_set_power_mode(SYSCTL_POWER_BANK1, SYSCTL_POWER_V33);
    sysctl_set_power_mode(SYSCTL_POWER_BANK2, SYSCTL_POWER_V33);
    sysctl_set_power_mode(SYSCTL_POWER_BANK3, SYSCTL_POWER_V33);
    sysctl_set_power_mode(SYSCTL_POWER_BANK4, SYSCTL_POWER_V33);
    sysctl_set_power_mode(SYSCTL_POWER_BANK5, SYSCTL_POWER_V33);
    sysctl_set_power_mode(SYSCTL_POWER_BANK6, SYSCTL_POWER_V33);
    sysctl_set_power_mode(SYSCTL_POWER_BANK7, SYSCTL_POWER_V33);
#endif
}

