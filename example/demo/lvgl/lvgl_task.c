/*
 * User Thread Instance (display task) Interface
 *
 * File Name:   lvgl_task.c
 * Author:      Yang Yujun
 * E-mail:      <yujiantianhu@163.com>
 * Created on:  2024.11.17
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
#define LVGL_TASK_THREAD_STACK_SIZE                       THREAD_STACK_PAGE(1)    /*!< 1 page (4kbytes) */

/*!< The globals */
static tid_t g_lvgl_task_tid;
static struct thread_attr sgrt_lvgl_task_attr;
static kuint32_t g_lvgl_task_stack[LVGL_TASK_THREAD_STACK_SIZE];

/*!< API functions */
/*!
 * @brief  display task
 * @param  none
 * @retval none
 * @note   do display
 */
static void *lvgl_task_entry(void *args)
{
    struct fwk_disp_ctrl sgrt_dctrl;
    struct fwk_disp_info sgrt_disp;

    sgrt_dctrl.sprt_di = &sgrt_disp;
    lvgl_task_startup(&sgrt_dctrl);

    for (;;)
    {
        lvgl_task(&sgrt_dctrl);
        schedule_delay_ms(200);
    }

    return args;
}

/*!
 * @brief	create display app task
 * @param  	none
 * @retval 	error code
 * @note   	none
 */
kint32_t lvgl_task_init(void)
{
    struct thread_attr *sprt_attr = &sgrt_lvgl_task_attr;
    kint32_t retval;

	sprt_attr->detachstate = THREAD_CREATE_JOINABLE;
	sprt_attr->inheritsched	= THREAD_INHERIT_SCHED;
	sprt_attr->schedpolicy = THREAD_SCHED_FIFO;

    /*!< thread stack */
	thread_set_stack(sprt_attr, mrt_nullptr, &g_lvgl_task_stack[0], sizeof(g_lvgl_task_stack));
    /*!< lowest priority */
	thread_set_priority(sprt_attr, THREAD_PROTY_DEFAULT);
    /*!< default time slice */
    thread_set_time_slice(sprt_attr, 100);

    /*!< register thread */
    retval = thread_create(&g_lvgl_task_tid, sprt_attr, lvgl_task_entry, mrt_nullptr);
    if (!retval)
        thread_set_name(g_lvgl_task_tid, "lvgl_task_entry");

    return retval;
}

/*!< end of file */
