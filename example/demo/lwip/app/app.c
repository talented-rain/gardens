/*
 * User Thread Instance (lwip task) Interface
 *
 * File Name:   app.c
 * Author:      Yang Yujun
 * E-mail:      <yujiantianhu@163.com>
 * Created on:  2024.11.21
 *
 * Copyright (c) 2024   Yang Yujun <yujiantianhu@163.com>
 *
 */

/*!< The globals */
#include <platform/fwk_fcntl.h>
#include <platform/net/fwk_ether.h>
#include <platform/net/fwk_if.h>

#include "app.h"

/*!< The defines */
#define DEFAULT_IP_ADDRESS          { 192, 168, 253, 206 }
#define DEFAULT_IP_MASK             { 255, 255, 255, 0 }
#define DEFAULT_GW_ADDRESS          { 192, 168, 253, 1 }

/*!< The globals */

/*!< API functions */

/*!
 * @brief  start up
 * @param  sprt_dctrl
 * @retval none
 * @note   none
 */
void lwip_task_startup(void *args)
{

}

/*!
 * @brief  main
 * @param  args
 * @retval none
 * @note   none
 */
void lwip_task(void *args)
{

}

/*!< end of file */
