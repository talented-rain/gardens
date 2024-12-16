/*
 * Kernel Scheduler Defines
 *
 * File Name:   sched.h
 * Author:      Yang Yujun
 * E-mail:      <yujiantianhu@163.com>
 * Created on:  2023.07.09
 *
 * Copyright (c) 2023   Yang Yujun <yujiantianhu@163.com>
 *
 */

#ifndef __KEL_SCHED_H_
#define __KEL_SCHED_H_

/*!< The includes */
#include <kernel/kernel.h>
#include <kernel/context.h>
#include <kernel/thread.h>
#include <kernel/spinlock.h>

/*!< The defines */
struct spin_lock;

#define REAL_THREAD_NAME_SIZE                    (32)

struct real_thread
{
    /*!< thread name */
    kchar_t name[REAL_THREAD_NAME_SIZE];

    /*!< thread id */
    kuint32_t tid;

    /*!< refer to "__ERT_KEL_BASIC_STATUS" */
    kuint32_t status;
    kuint32_t to_status;

    /*!< thread entry */
    void *(*start_routine) (void *);
    struct real_thread_attr *sprt_attr;
    void *ptr_args;

    /*!< thread list (to ready/suspend/sleep list) */
    struct list_head sgrt_link;

    /*!< thread time slice (period = sprt_attr->sgrt_param.mrt_sched_init_budget) */
    kutime_t expires;

    /*!< refer to "__ERT_REAL_THREAD_SIGNALS" */
    kuint32_t flags;

    struct spin_lock sgrt_lock;
};

#define mrt_thread_set_flags(signal, sprt_tsk)	\
    do {	\
        sprt_tsk->flags |= mrt_bit(signal);	\
    } while (0)

#define mrt_thread_clr_flags(signal, sprt_tsk)	\
    do {	\
        sprt_tsk->flags &= ~mrt_bit(signal);	\
    } while (0)

#define mrt_thread_is_flags(signal, sprt_tsk)						(!!((sprt_tsk)->flags & mrt_bit(signal)))

/*!< thread manage table */
struct scheduler_table
{
    kint32_t max_tidarr;											/*!< = REAL_THREAD_MAX_NUM */
    kint32_t max_tids; 												/*!< = REAL_THREAD_MAX_NUM + count of sprt_tids */
    kint32_t max_tidset;											/*!< the max tid */
    kint32_t ref_tidarr; 											/*!< number of allocated descriptors in sprt_tid_array */

    struct {
        kutype_t cnt_out;											/*!< when sched_cnt is over (~0), cnt_out++ */
        kutype_t sched_cnt;											/*!< schedule counter, max is ~0 */
    } sgrt_cnt;

    struct list_head sgrt_ready;									/*!< ready list head (manage all ready thread) */
    struct list_head sgrt_suspend;									/*!< suspend list head (manage all suspend thread) */
    struct list_head sgrt_sleep;									/*!< sleep list head (manage all sleepy thread) */

    struct real_thread *sprt_work;									/*!< current thread (status is running) */

    struct real_thread **sprt_tids;									/*!< if sprt_tid_array is up to max, new thread form mempool */
    struct real_thread *sprt_tid_array[REAL_THREAD_MAX_NUM];		/*!< thread maximum, tid = 0 ~ REAL_THREAD_MAX_NUM */

    struct spin_lock sgrt_lock;

#define __REAL_THREAD_MAX_STATS					((kutype_t)(~0))
#define __REAL_THREAD_HANDLER(ptr, tid)			((ptr)->sprt_tid_array[(tid)])
#define __REAL_THREAD_RUNNING_LIST(ptr)			((ptr)->sprt_work)
#define __REAL_THREAD_READY_LIST(ptr)			(&((ptr)->sgrt_ready))
#define __REAL_THREAD_SUSPEND_LIST(ptr)			(&((ptr)->sgrt_suspend))
#define __REAL_THREAD_SLEEP_LIST(ptr)			(&((ptr)->sgrt_sleep))
};

/*!< The globals */

/*!< The functions */
TARGET_EXT struct real_thread *get_current_thread(void);
TARGET_EXT struct list_head *get_ready_thread_table(void);
TARGET_EXT struct real_thread *get_thread_handle(tid_t tid);
TARGET_EXT void real_thread_set_name(tid_t tid, const kchar_t *name);
TARGET_EXT void real_thread_set_self_name(const kchar_t *name);
TARGET_EXT void real_thread_set_state(struct real_thread *sprt_thread, kuint32_t state);
TARGET_EXT struct spin_lock *scheduler_lock(void);
TARGET_EXT tid_t get_unused_tid_from_scheduler(kuint32_t i_start, kuint32_t count);
TARGET_EXT kuint64_t scheduler_stats_get(void);
TARGET_EXT void schedule_self_suspend(void);
TARGET_EXT kint32_t schedule_thread_suspend(tid_t tid);
TARGET_EXT kint32_t schedule_thread_wakeup(tid_t tid);

TARGET_EXT kbool_t is_ready_thread_empty(void);
TARGET_EXT kbool_t is_suspend_thread_empty(void);
TARGET_EXT kbool_t is_sleep_thread_empty(void);
TARGET_EXT struct real_thread *get_first_ready_thread(void);
TARGET_EXT struct real_thread *get_first_suspend_thread(void);
TARGET_EXT struct real_thread *get_first_sleep_thread(void);

TARGET_EXT kint32_t schedule_thread_switch(tid_t tid);
TARGET_EXT kint32_t register_new_thread(struct real_thread *sprt_thread, tid_t tid);
TARGET_EXT void __real_thread_init_before(void);
TARGET_EXT struct scheduler_context *__schedule_thread(void);
TARGET_EXT void schedule_thread(void);

/*!< The defines */
#define mrt_current                             get_current_thread()
#define mrt_tid_handle(tid)                     get_thread_handle(tid)
#define mrt_tid_attr(tid)                       real_thread_attr_get(tid)

/*!< API functions */
/*!
 * @brief   get thread state
 * @param   sprt_thread
 * @retval  status
 * @note    none
 */
static inline kbool_t real_thread_state_pending(struct real_thread *sprt_thread)
{
    kbool_t is_wakeup, is_killed;

    spin_lock_irqsave(&sprt_thread->sgrt_lock);
    is_wakeup = mrt_thread_is_flags(NR_THREAD_SIG_WAKEUP, sprt_thread);
    is_killed = mrt_thread_is_flags(NR_THREAD_SIG_KILL, sprt_thread);

    mrt_barrier();

    mrt_thread_clr_flags(NR_THREAD_SIG_WAKEUP, sprt_thread);
    mrt_thread_clr_flags(NR_THREAD_SIG_KILL, sprt_thread);
    spin_unlock_irqrestore(&sprt_thread->sgrt_lock);

    return (is_wakeup || is_killed);
}

/*!
 * @brief   set thread state
 * @param   sprt_thread, state, mode
 * @retval  none
 * @note    none
 */
static inline void real_thread_state_signal(struct real_thread *sprt_thread, kuint32_t state, kbool_t mode)
{
    spin_lock_irqsave(&sprt_thread->sgrt_lock);

    if (mode)
        mrt_thread_set_flags(state, sprt_thread);
    else
        mrt_thread_clr_flags(state, sprt_thread);
    
    spin_unlock_irqrestore(&sprt_thread->sgrt_lock);
}

#endif /* __KEL_SCHED_H_ */
