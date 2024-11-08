/*
 * ZYNQ7 Common Definition
 *
 * File Name:   zynq7_common.c
 * Author:      Yang Yujun
 * E-mail:      <yujiantianhu@163.com>
 * Created on:  2024.06.19
 *
 * Copyright (c) 2024   Yang Yujun <yujiantianhu@163.com>
 *
 */

/*!< The includes */
#include <common/time.h>
#include "zynq7_common.h"

/*!< The defines */
#define SYSTEM_CLK_FREQ_CNT_PER_MS                          (0x7ffU)
#define SYSTEM_CLK_FREQ_CNT_PER_S                          	(SYSTEM_CLK_FREQ_CNT_PER_MS * 1000)
#define SYSTEM_CLK_FREQ_CNT_PER_US                          (SYSTEM_CLK_FREQ_CNT_PER_MS / 1000)

/*!< API function */
/*!
 * @brief   delay_s
 * @param   n_s
 * @retval  none
 * @note    delay n_s seconds
 */
void delay_s(kuint32_t n_s)
{
	while (n_s--)
		delay_cnt(SYSTEM_CLK_FREQ_CNT_PER_S);
}

/*!
 * @brief   delay_ms
 * @param   n_ms
 * @retval  none
 * @note    delay n_ms miliseconds
 */
void delay_ms(kuint32_t n_ms)
{
	while (n_ms--)
		delay_cnt(SYSTEM_CLK_FREQ_CNT_PER_MS);
}

/*!
 * @brief   delay_us
 * @param   n_us
 * @retval  none
 * @note    delay n_us microseconds
 */
void delay_us(kuint32_t n_us)
{
	while (n_us--)
		delay_cnt(SYSTEM_CLK_FREQ_CNT_PER_US);
}


/* end of file */
