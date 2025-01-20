/*
 * User Thread Instance (net task) Interface
 *
 * File Name:   lwip_task.c
 * Author:      Yang Yujun
 * E-mail:      <yujiantianhu@163.com>
 * Created on:  2024.11.21
 *
 * Copyright (c) 2024   Yang Yujun <yujiantianhu@163.com>
 *
 */

/*!< The globals */
#include <common/basic_types.h>
#include <common/error_types.h>
#include <common/generic.h>
#include <common/io_stream.h>
#include <platform/fwk_fcntl.h>
#include <platform/video/fwk_fbmem.h>
#include <platform/video/fwk_font.h>
#include <platform/video/fwk_disp.h>
#include <platform/video/fwk_rgbmap.h>
#include <platform/video/fwk_bitmap.h>
#include <kernel/kernel.h>
#include <kernel/sched.h>
#include <kernel/thread.h>
#include <kernel/sleep.h>
#include <kernel/mutex.h>
#include <kernel/mailbox.h>
#include <fs/fs_intr.h>

#include "../../task.h"
#include "../demo_task.h"
#include "app/app.h"

using namespace tsk;

/*!< The defines */
#define LWIP_TASK_STACK_SIZE                        THREAD_STACK_PAGE(1)    /*!< 1 page (4kbytes) */

/*!< The globals */
static crt_lwip_data_t sgrt_lwip_task_data;

/*!< API functions */
/*!
 * @brief  net task
 * @param  none
 * @retval none
 * @note   do display
 */
static void *lwip_task_entry(void *args)
{
    crt_lwip_data_t &sgrt_data = sgrt_lwip_task_data;
    lwip_task_startup(sgrt_data);

    for (;;)
    {
        lwip_task(sgrt_data);
        schedule_delay_ms(200);
    }

    return args;
}

/*!
 * @brief	create net app task
 * @param  	none
 * @retval 	error code
 * @note   	none
 */
kint32_t lwip_task_init(void)
{
    static kuint8_t g_lwip_task_stack[LWIP_TASK_STACK_SIZE];

    crt_task_t *cprt_task = new crt_task_t("lwip task", 
                                            lwip_task_entry, 
                                            g_lwip_task_stack, 
                                            sizeof(g_lwip_task_stack),
                                            THREAD_PROTY_DEFAULT,
                                            100);
    if (!cprt_task)
        return -ER_FAILD;

    struct mailbox &sgrt_mb = cprt_task->get_mailbox();
    mailbox_init(&sgrt_mb, cprt_task->get_self(), "lwip-task-mailbox");

    return ER_NORMAL;
}

/*!< end of file */
