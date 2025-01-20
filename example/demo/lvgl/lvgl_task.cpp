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
#include <kernel/kernel.h>
#include <kernel/sched.h>
#include <kernel/thread.h>
#include <kernel/sleep.h>
#include <kernel/mutex.h>
#include <kernel/mailbox.h>

#include "../../task.h"
#include "../demo_task.h"

#include "porting/lv_port_disp.h"
#include "porting/lv_port_fs.h"
#include "app/app.h"

using namespace tsk;

/*!< The defines */
#define LVGL_TASK_STACK_SIZE                    THREAD_STACK_PAGE(1)    /*!< 1 page (4kbytes) */

/*!< The globals */

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

    lv_init();
    lv_port_disp_init(&sgrt_dctrl);
    lv_port_fs_init();

    lvgl_task_setup(&sgrt_dctrl);
    schedule_delay_ms(1);

    for (;;)
    {
        lvgl_task(&sgrt_dctrl);
        schedule_delay_ms(100);
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
    static kuint8_t g_lvgl_task_stack[LVGL_TASK_STACK_SIZE];

    crt_task_t *cprt_task = new crt_task_t("lvgl task", 
                                            lvgl_task_entry, 
                                            g_lvgl_task_stack, 
                                            sizeof(g_lvgl_task_stack),
                                            THREAD_PROTY_DEFAULT,
                                            100);
    if (!cprt_task)
        return -ER_FAILD;

    struct mailbox &sgrt_mb = cprt_task->get_mailbox();
    mailbox_init(&sgrt_mb, cprt_task->get_self(), "lvgl-task-mailbox");

    return ER_NORMAL;
}

/*!< end of file */
