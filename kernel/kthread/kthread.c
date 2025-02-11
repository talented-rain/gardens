/*
 * Kernel Thread Instance (kernel task) Interface
 *
 * File Name:   kthread.c
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
#include <kernel/spinlock.h>

/*!< The defines */
#define KERL_THREAD_STACK_SIZE                          THREAD_STACK_PAGE(1)   /*!< 1 page (4 kbytes) */

/*!< The globals */
extern kuint32_t g_asm_sched_flag;

static struct thread_attr sgrt_kthread_attr;
static kuint32_t g_kthread_stack[KERL_THREAD_STACK_SIZE];
static struct timer_list sgrt_kthread_timer;
static struct spin_lock sgrt_kthread_spinlock;

/*!< API functions */
/*!
 * @brief	kernel thread scheduler
 * @param  	args: timer
 * @retval 	none
 * @note   	check priority and time slice, set g_asm_sched_flag to true or false
 */
static void kthread_schedule_timeout(kuint32_t args)
{
    struct timer_list *sprt_tim = (struct timer_list *)args;
    struct thread *sprt_work, *sprt_ready;
    kuint32_t work_prio, next_prio;

    sprt_work = mrt_current;
    
    /*!< --------------------------------------------------------- */
    /*!< check time slice (when the time slice is not exhuasted, current cannot be preempted) */
    if (mrt_time_before(jiffies, sprt_work->expires))
        goto END;

    /*!< automatic tracking of time-slice */
    sprt_work->expires = ((JIFFIES_MAX - jiffies) <= THREAD_PREEMPT_PERIOD) ? 0 : jiffies;
    
    /*!< --------------------------------------------------------- */
    /*!< check priority */
    sprt_ready = get_first_ready_thread();
    if (!sprt_ready)
        goto END;

    work_prio = thread_get_priority(sprt_work->sprt_attr);
    next_prio = thread_get_priority(sprt_ready->sprt_attr);
   
    /*!< there is a higher priority thread ready */
    if (__THREAD_IS_LOW_PRIO(work_prio, next_prio))
        g_asm_sched_flag = true;
    
END:
    mod_timer(sprt_tim, jiffies + msecs_to_jiffies(THREAD_PREEMPT_PERIOD));
}

/*!
 * @brief	record system run ticks
 * @param  	none
 * @retval 	none
 * @note   	none
 */
static void kthread_systime_record(void)
{
    static kutime_t systime = 0;

    if (systime != jiffies)
    {
        msecs_to_timeclock(&sgrt_systime_clock, jiffies_to_msecs(systime));
        systime = SYS_RUNTICK;
    }
}

/*!
 * @brief	kernel thread entry
 * @param  	args: NULL normally
 * @retval 	none
 * @note   	create all kernel thread
 */
static void *kthread_entry(void *args)
{
    struct timer_list *sprt_tim = &sgrt_kthread_timer;
    tid_t tid = mrt_current->tid;

    mrt_preempt_disable();
    
    thread_set_self_name(__FUNCTION__);
    spin_lock_init(&sgrt_kthread_spinlock);

#if CONFIG_PREEMPT
    setup_timer(sprt_tim, kthread_schedule_timeout, (kuint32_t)sprt_tim);
    sprt_tim->expires = jiffies + msecs_to_jiffies(THREAD_PREEMPT_PERIOD);
    add_timer(sprt_tim);
#endif

    kworker_init();                         /*!< create kworker task */

    print_info("%s is enter, which tid is: %d\n", __FUNCTION__, tid);
    mrt_preempt_enable();

    /* platform initcall */
    run_platform_initcall();
    print_info("platform initialization finished\n");

    term_init();                            /*!< create term task */

    /*!< build application */
    init_proc_init();                       /*!< create init task */

    for (;;)
    {        
        kthread_systime_record();
        msleep(200);
    }

    return args;
}

/*!
 * @brief	create kernel thread
 * @param  	none
 * @retval 	error code
 * @note   	none
 */
kint32_t kthread_init(void)
{
    struct thread_attr *sprt_attr = &sgrt_kthread_attr;

	sprt_attr->detachstate = THREAD_CREATE_JOINABLE;
	sprt_attr->inheritsched	= THREAD_INHERIT_SCHED;
	sprt_attr->schedpolicy = THREAD_SCHED_FIFO;

    /*!< thread stack */
	thread_set_stack(sprt_attr, mrt_nullptr, g_kthread_stack, sizeof(g_kthread_stack));
    /*!< lowest priority */
	thread_set_priority(sprt_attr, THREAD_PROTY_KERNEL);
    /*!< default time slice */
    thread_set_time_slice(sprt_attr, THREAD_TIME_DEFUALT);

    /*!< register thread */
    return kernel_thread_base_create(sprt_attr, kthread_entry, mrt_nullptr);
}

/*!< end of file */
