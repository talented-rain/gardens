/*
 * User Thread Instance (tsc task) Interface
 *
 * File Name:   tsc_task.c
 * Author:      Yang Yujun
 * E-mail:      <yujiantianhu@163.com>
 * Created on:  2024.05.21
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
#include <platform/input/fwk_input.h>
#include <kernel/kernel.h>
#include <kernel/sched.h>
#include <kernel/thread.h>
#include <kernel/sleep.h>
#include <kernel/mutex.h>
#include <kernel/mailbox.h>

#include "../task.h"
#include "test_task.h"

using namespace tsk;
using namespace stream;

/*!< The defines */
#define TSC_TASK_STACK_SIZE                     THREAD_STACK_HALF(1)    /*!< 1/2 page (2kbytes) */

/*!< The globals */

/*!< API functions */
/*!
 * @brief  tsc task
 * @param  none
 * @retval none
 * @note   trun on/off led by timer
 */
static void *tsc_task_entry(void *args)
{
    kint32_t fd;
    struct fwk_input_event sgrt_event[4] = {};
    kssize_t retval;
    
    do 
    {
        fd = virt_open("/dev/input/event1", O_RDONLY);
        if (fd < 0)
            schedule_delay_ms(200);

    } while (fd < 0);

    for (;;)
    {
        memset(&sgrt_event[0], 0, sizeof(sgrt_event));

        retval = virt_read(fd, &sgrt_event[0], sizeof(sgrt_event));
        if ((retval < 0))
            goto END;

        cout << __FUNCTION__ 
             << ": key: "   << sgrt_event[0].value 
             << ", abs_x: " << sgrt_event[1].value 
             << ", abs_y: " << sgrt_event[2].value 
             << endl;
        
END:
        schedule_delay_ms(200);
    }

    virt_close(fd);
    return args;
}

/*!
 * @brief	create tsc app task
 * @param  	none
 * @retval 	error code
 * @note   	none
 */
kint32_t tsc_task_init(void)
{
    static kuint8_t g_tsc_task_stack[TSC_TASK_STACK_SIZE];

    crt_task_t *cprt_task = new crt_task_t("tsc task", 
                                            tsc_task_entry, 
                                            g_tsc_task_stack, 
                                            sizeof(g_tsc_task_stack));
    if (!cprt_task)
        return -ER_FAILD;

    struct mailbox &sgrt_mb = cprt_task->get_mailbox();
    mailbox_init(&sgrt_mb, cprt_task->get_self(), "tsc-task-mailbox");

    return ER_NORMAL;
}

/*!< end of file */
