/*
 * IMX6ULL Board Common Header
 *
 * File Name:   imx6_common.h
 * Author:      Yang Yujun
 * E-mail:      <yujiantianhu@163.com>
 * Created on:  2023.09.11
 *
 * Copyright (c) 2023   Yang Yujun <yujiantianhu@163.com>
 *
 */

#ifndef __IMX6_COMMON_H
#define __IMX6_COMMON_H

/*!< The includes */
#include <common/generic.h>
#include <common/io_stream.h>
#include <imx6/imx6ull_clocks.h>
#include <imx6/imx6ull_pins.h>
#include <imx6/imx6ull_periph.h>
#include <imx6/imx6ull_irqvector.h>

/*!< The functions */
TARGET_EXT void imx6ull_led_init(void);
TARGET_EXT void imx6ull_console_init(void);
TARGET_EXT void imx6ull_sdmmc_init(void);
TARGET_EXT void imx6ull_systick_init(void);

#endif
