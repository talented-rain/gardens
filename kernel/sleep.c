/*
 * Kernel Time Interface
 *
 * File Name:   time.c
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
#include <kernel/spinlock.h>

/*!< The defines */


/*!< The functions */


/*!< API functions */
/*!
 * @brief   suspend status over, wake it up
 * @param   args
 * @retval  none
 * @note    timeout function
 */
static void thread_sleep_timeout(kuint32_t args)
{
    struct thread *sprt_thread = (struct thread *)args;
    struct spin_lock *sprt_lock = scheduler_lock();

    if (spin_is_locked(sprt_lock))
		return;

    if (sprt_thread->status == NR_THREAD_SUSPEND)
        schedule_thread_wakeup(sprt_thread->tid);
}

/*!
 * @brief   setup timer for sleeping
 * @param   count
 * @retval  none
 * @note    delay and schedule (current thread may convert to suspend status)
 */
void schedule_timeout(kutime_t count)
{
    struct timer_list sgrt_tm;
    struct thread *sprt_thread = mrt_current;
    struct spin_lock *sprt_lock = scheduler_lock();

    if (!count)
        schedule_thread();
	
	spin_lock_irqsave(sprt_lock);
    setup_timer(&sgrt_tm, thread_sleep_timeout, (kuint32_t)sprt_thread);
    mod_timer(&sgrt_tm, count);
    spin_unlock_irqrestore(sprt_lock);
    
    /*!< suspend current thread, and schedule others */
    schedule_self_suspend();
    
    spin_lock_irqsave(sprt_lock);
    del_timer(&sgrt_tm);
    spin_unlock_irqrestore(sprt_lock);
}

/*!
 * @brief   sleep (unit: s)
 * @param   seconds
 * @retval  none
 * @note    delay and schedule (current thread may convert to suspend status)
 */
kuint32_t sleep(kuint32_t seconds)
{
    kutime_t count = jiffies + secs_to_jiffies(seconds);

    if (mrt_likely(mrt_current))
    {
    #if CONFIG_ROLL_POLL
        while (mrt_time_after(count, jiffies))
            schedule_thread();

    #else
        if (mrt_time_after(count, jiffies))
            schedule_timeout(count);
        
    #endif
    }
    else
    {
        /*!< wait_secs(seconds); */
        while (mrt_time_after(count, jiffies));
    }

    return (kuint32_t)count;
}

/*!
 * @brief   sleep (unit: ms)
 * @param   milseconds
 * @retval  none
 * @note    delay and schedule (current thread may convert to suspend status)
 */
kuint32_t msleep(kuint32_t milseconds)
{
    kutime_t count = jiffies + msecs_to_jiffies(milseconds);
    
    if (mrt_likely(mrt_current))
    {
    #if CONFIG_ROLL_POLL
        while (mrt_time_after(count, jiffies))
            schedule_thread();

    #else
        if (mrt_time_after(count, jiffies))
            schedule_timeout(count);
        
    #endif
    }
    else
    {
        /*!< wait_msecs(milseconds); */
        while (mrt_time_after(count, jiffies));
    }

    return (kuint32_t)count;
}

/*!
 * @brief   usleep (unit: us)
 * @param   useconds
 * @retval  none
 * @note    delay and schedule (current thread may convert to suspend status)
 */
kint32_t usleep(kuint32_t useconds)
{
    kutime_t count = jiffies + usecs_to_jiffies(useconds);
    
    if (mrt_likely(mrt_current))
    {
    #if CONFIG_ROLL_POLL
        while (mrt_time_after(count, jiffies))
            schedule_thread();

    #else
        if (mrt_time_after(count, jiffies))
            schedule_timeout(count);
        
    #endif
    }
    else
    {
        /*!< wait_usecs(useconds); */
        while (mrt_time_after(count, jiffies));
    }

    return (kint32_t)count;
}

/*!< end of file */
