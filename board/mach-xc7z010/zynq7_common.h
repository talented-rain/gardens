/*
 * ZYNQ7 Board Common Header
 *
 * File Name:   zynq7_common.h
 * Author:      Yang Yujun
 * E-mail:      <yujiantianhu@163.com>
 * Created on:  2024.06.19
 *
 * Copyright (c) 2024   Yang Yujun <yujiantianhu@163.com>
 *
 */

#ifndef __ZYNQ7_COMMON_H
#define __ZYNQ7_COMMON_H

/*!< The includes */
#include <common/generic.h>
#include <common/io_stream.h>
#include <configs/configs.h>
#include <asm/zynq7/zynq7_periph.h>

/*!< The functions */
TARGET_EXT void zynq7_led_init(void);
TARGET_EXT void zynq7_console_init(void);
TARGET_EXT void zynq7_systick_init(void);
TARGET_EXT void zynq7_sdmmc_init(void);
TARGET_EXT void zynq7_hdmi_init(void);

#endif
