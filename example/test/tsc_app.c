/*
 * User Thread Instance (tsc task) Interface
 *
 * File Name:   tsc_app.c
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

#include "../thread_table.h"

/*!< The defines */
#define TSCAPP_THREAD_STACK_SIZE                          REAL_THREAD_STACK_HALF(1)    /*!< 1/2 page (2kbytes) */

/*!< The globals */
static real_thread_t g_tsc_app_tid;
static struct real_thread_attr sgrt_tsc_app_attr;
static kuint32_t g_tsc_app_stack[TSCAPP_THREAD_STACK_SIZE];

/*!< API functions */
/*!
 * @brief  tsc task
 * @param  none
 * @retval none
 * @note   trun on/off led by timer
 */
static void *tsc_app_entry(void *args)
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

        print_info("%s: key: %d, abs_x: %d, abs_y: %d\n", __FUNCTION__, 
                        sgrt_event[0].value, sgrt_event[1].value, sgrt_event[2].value);
        
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
kint32_t tsc_app_init(void)
{
    struct real_thread_attr *sprt_attr = &sgrt_tsc_app_attr;
    kint32_t retval;

	sprt_attr->detachstate = REAL_THREAD_CREATE_JOINABLE;
	sprt_attr->inheritsched	= REAL_THREAD_INHERIT_SCHED;
	sprt_attr->schedpolicy = REAL_THREAD_SCHED_FIFO;

    /*!< thread stack */
	real_thread_set_stack(sprt_attr, mrt_nullptr, g_tsc_app_stack, sizeof(g_tsc_app_stack));
    /*!< lowest priority */
	real_thread_set_priority(sprt_attr, __THREAD_HIGHER_DEFAULT(0));
    /*!< default time slice */
    real_thread_set_time_slice(sprt_attr, REAL_THREAD_TIME_DEFUALT);

    /*!< register thread */
    retval = real_thread_create(&g_tsc_app_tid, sprt_attr, tsc_app_entry, mrt_nullptr);
    if (!retval)
        real_thread_set_name(g_tsc_app_tid, "tsc_app_entry");

    return retval;
}

/*!< end of file */
