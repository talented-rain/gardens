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

#include "../../thread_table.h"
#include "app/app.h"

/*!< The defines */
#define LWIP_TASK_THREAD_STACK_SIZE                       REAL_THREAD_STACK_PAGE(1)    /*!< 1 page (4kbytes) */

/*!< The globals */
static real_thread_t g_lwip_task_tid;
static struct real_thread_attr sgrt_lwip_task_attr;
static kuint32_t g_lwip_task_stack[LWIP_TASK_THREAD_STACK_SIZE];

static struct lwip_task_data sgrt_lwip_task_data;

/*!< API functions */
/*!
 * @brief  net task
 * @param  none
 * @retval none
 * @note   do display
 */
static void *lwip_task_entry(void *args)
{
    lwip_task_startup(&sgrt_lwip_task_data);

    for (;;)
    {
        lwip_task(&sgrt_lwip_task_data);
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
    struct real_thread_attr *sprt_attr = &sgrt_lwip_task_attr;
    kint32_t retval;

	sprt_attr->detachstate = REAL_THREAD_CREATE_JOINABLE;
	sprt_attr->inheritsched	= REAL_THREAD_INHERIT_SCHED;
	sprt_attr->schedpolicy = REAL_THREAD_SCHED_FIFO;

    /*!< thread stack */
	real_thread_set_stack(sprt_attr, mrt_nullptr, &g_lwip_task_stack[0], sizeof(g_lwip_task_stack));
    /*!< lowest priority */
	real_thread_set_priority(sprt_attr, REAL_THREAD_PROTY_DEFAULT);
    /*!< default time slice */
    real_thread_set_time_slice(sprt_attr, 100);

    /*!< register thread */
    retval = real_thread_create(&g_lwip_task_tid, sprt_attr, lwip_task_entry, mrt_nullptr);
    if (!retval)
        real_thread_set_name(g_lwip_task_tid, "lwip_task_entry");

    return retval;
}

/*!< end of file */
