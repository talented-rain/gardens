/*
 * Kernel Thread Instance (init task) Interface
 *
 * File Name:   init_proc.c
 * Author:      Yang Yujun
 * E-mail:      <yujiantianhu@163.com>
 * Created on:  2024.04.01
 *
 * Copyright (c) 2024   Yang Yujun <yujiantianhu@163.com>
 *
 */

/*!< The globals */
#include <kernel/kernel.h>
#include <kernel/sched.h>
#include <kernel/thread.h>
#include <kernel/sleep.h>
#include <kernel/instance.h>

/*!< The defines */
#define INIT_THREAD_STACK_SIZE                          REAL_THREAD_STACK_HALF(1)    /*!< 1/2 page (2 kbytes) */

/*!< The globals */
static struct real_thread_attr sgrt_init_proc_attr;
static kuint32_t g_init_proc_stack[INIT_THREAD_STACK_SIZE];

/*!< API functions */
/*!
 * @brief	init thread entry
 * @param  	args: NULL normally
 * @retval 	none
 * @note   	create all user thread
 */
static void *init_proc_entry(void *args)
{
    real_thread_set_name(__FUNCTION__);
    init_real_thread_table();

    print_info("%s is enter, which tid is: %d\n", __FUNCTION__, mrt_current->tid);

    for (;;)
    {  
        schedule_delay_ms(200);
    }

    return args;
}

/*!
 * @brief	create init thread
 * @param  	none
 * @retval 	error code
 * @note   	none
 */
kint32_t init_proc_init(void)
{
    struct real_thread_attr *sprt_attr = &sgrt_init_proc_attr;

	sprt_attr->detachstate = REAL_THREAD_CREATE_JOINABLE;
	sprt_attr->inheritsched	= REAL_THREAD_INHERIT_SCHED;
	sprt_attr->schedpolicy = REAL_THREAD_SCHED_FIFO;

    /*!< thread stack */
	real_thread_set_stack(sprt_attr, mrt_nullptr, g_init_proc_stack, sizeof(g_init_proc_stack));
    /*!< lowest priority */
	real_thread_set_priority(sprt_attr, REAL_THREAD_PROTY_INIT);
    /*!< default time slice */
    real_thread_set_time_slice(sprt_attr, REAL_THREAD_TIME_DEFUALT);

    /*!< register thread */
    return kernel_thread_init_create(sprt_attr, init_proc_entry, mrt_nullptr);
}

/*!< end of file */
