/*
 * Kernel Thread Wait Queue Defines
 *
 * File Name:   wakequeue.h
 * Author:      Yang Yujun
 * E-mail:      <yujiantianhu@163.com>
 * Created on:  2024.05.29
 *
 * Copyright (c) 2024   Yang Yujun <yujiantianhu@163.com>
 *
 */

#ifndef __KERNEL_WAIT_H_
#define __KERNEL_WAIT_H_

/*!< The includes */
#include <kernel/kernel.h>
#include <kernel/thread.h>
#include <kernel/sched.h>
#include <kernel/sleep.h>
#include <kernel/spinlock.h>

/*!< The defines */
#define FWK_WAIT_NO_CONDION                                     (0)

typedef struct wait_queue_head
{
    struct spin_lock sgrt_lock;
    struct list_head sgrt_task;

} srt_wait_queue_head_t;

#define __WAITQUEUE_HEAD_INITIALIZER(name)  \
{   \
    .sgrt_lock = SPIN_LOCK_INIT(),  \
    .sgrt_task = LIST_HEAD_INIT(&(name)->sgrt_task) \
}

#define DECALRE_WAITQUEUE_HEAD(name)    \
    srt_wait_queue_head_t name = __WAITQUEUE_HEAD_INITIALIZER(name)

typedef struct wait_queue
{
    struct thread *sprt_task;
    struct list_head sgrt_link;

} srt_wait_queue_t;

#define __WAITQUEUE_INITIALIZER(tsk)  \
{    \
	.sprt_task	= tsk,  \
	.sgrt_link	= { mrt_nullptr, mrt_nullptr }    \
}

#define DECLARE_WAITQUEUE(name, tsk)    \
    srt_wait_queue_t name = __WAITQUEUE_INITIALIZER(tsk)

/*!< The functions */
TARGET_EXT void init_waitqueue_head(struct wait_queue_head *sprt_wqh);
TARGET_EXT void add_wait_queue(struct wait_queue_head *sprt_wqh, struct wait_queue *sprt_wq);
TARGET_EXT void remove_wait_queue(struct wait_queue_head *sprt_wqh, struct wait_queue *sprt_wq);
TARGET_EXT void wake_up_common(struct wait_queue_head *sprt_wqh, kuint32_t state);

/*!< The defines */
#define __wait_is_interruptible(sprt_wq)                (mrt_thread_is_flags(NR_THREAD_SIG_INTR, (sprt_wq)->sprt_task))

#define __wait_event(sprt_wqh, condition, sig_enable, func) \
({  \
    DECLARE_WAITQUEUE(sgrt_wq, mrt_current);  \
    const kuint32_t state = (sig_enable) ? NR_THREAD_SIG_INTR : NR_THREAD_SIG_NORMAL;  \
    \
    if (!sgrt_wq.sprt_task)   \
        while (!(condition)); \
    else {  \
        init_list_head(&sgrt_wq.sgrt_link); \
        add_wait_queue(sprt_wqh, &sgrt_wq); \
        thread_state_signal(sgrt_wq.sprt_task, state, true);   \
        \
        for (;;) {   \
            \
            /*!< condition is satisfied */  \
            if (condition)  \
                break;  \
            \
            /*!< signal pending (by calling "wake_up") */   \
            if (__wait_is_interruptible(&sgrt_wq) && thread_state_pending(sgrt_wq.sprt_task)) \
                break;  \
            \
            func; \
            \
        }   \
        \
        thread_state_signal(sgrt_wq.sprt_task, state, false);   \
        remove_wait_queue(sprt_wqh, &sgrt_wq);  \
    }   \
})

#define wait_event(sprt_wqh, condition) \
    do {    \
        if (condition)  \
            break;  \
        (void)__wait_event(sprt_wqh, condition, 0, schedule_thread());    \
    } while (0)

#define wait_event_timeout(sprt_wqh, condition, timeout) \
    do {    \
        if (condition)  \
            break;  \
        (void)__wait_event(sprt_wqh, condition, 0, schedule_timeout(timeout));    \
    } while (0)

#define wait_event_interruptible(sprt_wqh, condition) \
    do {    \
        if (condition)  \
            break;  \
        (void)__wait_event(sprt_wqh, condition, 1, schedule_thread());    \
    } while (0)

#define wait_event_interruptible_timeout(sprt_wqh, condition, timeout) \
    do {    \
        if (condition)  \
            break;  \
        (void)__wait_event(sprt_wqh, condition, 1, schedule_timeout(timeout));    \
    } while (0)

#define wake_up(sprt_wqh)   \
    do {    \
        wake_up_common(sprt_wqh, NR_THREAD_SIG_NORMAL);    \
    } while (0)

#define wake_up_interruptible(sprt_wqh)   \
    do {    \
        wake_up_common(sprt_wqh, NR_THREAD_SIG_INTR);    \
    } while (0)

#endif /* __KERNEL_WAIT_H_ */
