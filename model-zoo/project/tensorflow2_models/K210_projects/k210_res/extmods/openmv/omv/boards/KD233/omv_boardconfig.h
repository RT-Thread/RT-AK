/*
 * This file is part of the OpenMV project.
 *
 * Copyright (c) 2013-2019 Ibrahim Abdelkader <iabdalkader@openmv.io>
 * Copyright (c) 2013-2019 Kwabena W. Agyeman <kwagyeman@openmv.io>
 *
 * This work is licensed under the MIT license, see the file LICENSE for details.
 *
 * Board configuration and pin definitions.
 */
#ifndef __OMV_BOARDCONFIG_H__
#define __OMV_BOARDCONFIG_H__

#include <rtconfig.h>

// Architecture info
#define OMV_ARCH_STR            "K210" // 33 chars max
#define OMV_BOARD_TYPE          "KD233"
#define OMV_UNIQUE_ID_ADDR      0x1FF1E800

// Flash sectors for the bootloader.
// Flash FS sector, main FW sector, max sector.
#define OMV_FLASH_LAYOUT        {1, 2, 15}

// QSPI Flash layout for the bootloader.
// First block, maximum block, block size in bytes.
#define OMV_QSPIF_LAYOUT        {0, 511, 64*1024}

#define OMV_XCLK_MCO            (0U)
#define OMV_XCLK_TIM            (1U)

// Sensor external clock source.
#define OMV_XCLK_SOURCE         (OMV_XCLK_TIM)

// Sensor external clock timer frequency.
#define OMV_XCLK_FREQUENCY      (12000000)

// Sensor PLL register value.
#define OMV_OV7725_PLL_CONFIG   (0x41)  // x4

// Sensor Banding Filter Value
#define OMV_OV7725_BANDING      (0x7F)

// Bootloader LED GPIO port/pin
#define OMV_BOOTLDR_LED_PIN     (GPIO_PIN_1)
#define OMV_BOOTLDR_LED_PORT    (GPIOC)

// RAW buffer size
#ifndef OMV_RAW_BUF_SIZE
#define OMV_RAW_BUF_SIZE        (409600)
#endif

// Enable hardware JPEG
#define OMV_HARDWARE_JPEG       (0)

// Enable sensor drivers
#define OMV_ENABLE_OV2640       (1)
#define OMV_ENABLE_OV5640       (1)
#define OMV_ENABLE_OV7690       (0)
#define OMV_ENABLE_OV7725       (0)
#define OMV_ENABLE_OV9650       (0)
#define OMV_ENABLE_MT9V034      (0)
#define OMV_ENABLE_LEPTON       (0)
#define OMV_ENABLE_HM01B0       (0)
#define OMV_ENABLE_GC0328       (1)
#define OMV_ENABLE_GC0308       (1)
// Enable WiFi debug
#define OMV_ENABLE_WIFIDBG      (1)

// If buffer size is bigger than this threshold, the quality is reduced.
// This is only used for JPEG images sent to the IDE not normal compression.
#define JPEG_QUALITY_THRESH     (1920*1080*2)

// Low and high JPEG QS.
#define JPEG_QUALITY_LOW        50
#define JPEG_QUALITY_HIGH       90

// FB Heap Block Size
#define OMV_UMM_BLOCK_SIZE      256

// Core VBAT for selftests
#define OMV_CORE_VBAT           "3.3"

#define OMV_FB_SIZE             (400K)      // FB memory: header + VGA/GS image
#define OMV_FB_ALLOC_SIZE       (96*1024)       // minimum fb alloc size
#define OMV_STACK_SIZE          (64K)
#define OMV_HEAP_SIZE           (246K)

#define OMV_LINE_BUF_SIZE       (3 * 1024)  // Image line buffer round(640 * 2BPP * 2 buffers).
#define OMV_MSC_BUF_SIZE        (12K)       // USB MSC bot data
#define OMV_VFS_BUF_SIZE        (1K)        // VFS sturct + FATFS file buffer (624 bytes)
#define OMV_JPEG_BUF_SIZE       (32 * 1024) // IDE JPEG buffer (header + data).

#define __WFI()

#endif //__OMV_BOARDCONFIG_H__
