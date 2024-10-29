/*
 * Board Initialization
 *
 * File Name:   board_init.c
 * Author:      Yang Yujun
 * E-mail:      <yujiantianhu@163.com>
 * Created on:  2023.09.10
 *
 * Copyright (c) 2023   Yang Yujun <yujiantianhu@163.com>
 *
 */

/*!< The includes */
#include <boot/board_init.h>
#include "zynq7_common.h"

/*!< API function */
/*!
 * @brief   board_init_console
 * @param   none
 * @retval  none
 * @note    initial console
 */
kint32_t board_init_console(void)
{
	zynq7_console_init();

    /*!< Output Board Information */
    print_info("\n");
    print_info("Console Initial Finished ...\n");
    print_info("Welcome to Use HeavenFox OS!\n");

    return RET_BOOT_PASS;
}

/*!
 * @brief   board_init_systick
 * @param   none
 * @retval  none
 * @note    initial systick
 */
kint32_t board_init_systick(void)
{
    zynq7_systick_init();

    return RET_BOOT_PASS;
}

/*!
 * @brief   board_init_sdmmc
 * @param   none
 * @retval  none
 * @note    initial sdio
 */
kint32_t board_init_sdmmc(void)
{
	zynq7_sdmmc_init();

    return RET_BOOT_PASS;
}

/*!
 * @brief   board_init_light
 * @param   none
 * @retval  none
 * @note    initial led
 */
kint32_t board_init_light(void)
{
	zynq7_led_init();

    return RET_BOOT_PASS;
}

/* end of file */
