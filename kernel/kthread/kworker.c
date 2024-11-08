/*
 * Kernel Thread Instance (kernel task) Interface
 *
 * File Name:   kworker.c
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
#include <kernel/workqueue.h>

/*!< The defines */
#define KWORKER_THREAD_STACK_SIZE                       REAL_THREAD_STACK_HALF(1)    /*!< 1/2 page (2 kbytes) */

/*!< The globals */
static real_thread_t g_kworker_tid;
static struct real_thread_attr sgrt_kworker_attr;
static kuint32_t g_kworker_stack[KWORKER_THREAD_STACK_SIZE];

static DECLARE_WORKQUEUE(sgrt_kworker_wqh);

/*!< API functions */
/*!
 * @brief	add sprt_wq to sgrt_kworker_wqh
 * @param  	sprt_wq: new work
 * @retval 	none
 * @note   	none
 */
void schedule_work(struct workqueue *sprt_wq)
{
    queue_work(&sgrt_kworker_wqh, sprt_wq);
}

/*!
 * @brief	kernel worker thread entry
 * @param  	args: NULL normally
 * @retval 	none
 * @note   	none
 */
static void *kworker_entry(void *args)
{
    struct workqueue *sprt_wq;
    struct workqueue *sprt_temp;

    print_info("%s is enter, which tid is: %d\n", __FUNCTION__, mrt_current->tid);

    for (;;)
    {
        if (is_workqueue_empty(&sgrt_kworker_wqh))
            goto END;

        foreach_workqueue_safe(sprt_wq, sprt_temp, &sgrt_kworker_wqh)
        {
            if (sprt_wq->func)
                sprt_wq->func(sprt_wq);

            detach_work(sprt_wq);
        }

        continue;

END:
        schedule_delay_ms(200);
    }

    return args;
}

/*!
 * @brief	create kernel thread
 * @param  	none
 * @retval 	error code
 * @note   	none
 */
kint32_t kworker_init(void)
{
    struct real_thread_attr *sprt_attr = &sgrt_kworker_attr;
    kint32_t retval;

	sprt_attr->detachstate = REAL_THREAD_CREATE_JOINABLE;
	sprt_attr->inheritsched	= REAL_THREAD_INHERIT_SCHED;
	sprt_attr->schedpolicy = REAL_THREAD_SCHED_FIFO;

    /*!< thread stack */
	real_thread_set_stack(sprt_attr, mrt_nullptr, g_kworker_stack, sizeof(g_kworker_stack));
    /*!< lowest priority */
	real_thread_set_priority(sprt_attr, REAL_THREAD_PROTY_KWORKER);
    /*!< default time slice */
    real_thread_set_time_slice(sprt_attr, REAL_THREAD_TIME_DEFUALT);

    /*!< register thread */
    retval = kernel_thread_create(&g_kworker_tid, sprt_attr, kworker_entry, mrt_nullptr);
    if (!retval)
        real_thread_set_name(g_kworker_tid, "kworker_entry");

    return retval;
}

/*!< end of file */
