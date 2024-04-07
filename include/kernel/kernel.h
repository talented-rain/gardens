/*
 * Kernel Generic Interface Defines
 *
 * File Name:   kernel.h
 * Author:      Yang Yujun
 * E-mail:      <yujiantianhu@163.com>
 * Created on:  2023.07.09
 *
 * Copyright (c) 2023   Yang Yujun <yujiantianhu@163.com>
 *
 */

#ifndef __KERNEL_H
#define __KERNEL_H

/*!< The includes */
#include <common/generic.h>
#include <configs/configs.h>

/*!< The defines */
#define KEL_THREAD_USER									(0x1)	/*!< user thread */

/*!<
 * status move:
 *	ready ---> running
 *	running ---> ready/suspend/sleep
 *	suspend ---> ready
 *	sleep ---> ready/suspend (need to be awaked by another thread)
 */
enum __ERT_KEL_BASIC_STATUS
{
    NR_THREAD_NONE = 0,
    
	/*!< running status: thread is running  */
	NR_THREAD_RUNNING,

	/*!< ready status: thread is waitting for running */
	NR_THREAD_READY,

	/*!<
	 * suspend status: when thread is in delay or message-blocking stage, it is suspended;
	 * and returns to the ready state when the delay ends and the message is normal 
	 */
	NR_THREAD_SUSPEND,

	/*!<
	 * sleep status: thread is sleeping, e.g. thread is completely dormant, 
	 * and even if the ready condition is met, it will not be inserted into the ready list Can be awakened */
	NR_THREAD_SLEEP,

	/*!< numbers */
	NR_THREAD_STATUS_MAX
};

/*!< The globals */
TARGET_EXT kuint32_t g_sched_preempt_cnt;

#define mrt_preempt_cnt_dec()						COUNT_DEC(g_sched_preempt_cnt)
#define mrt_preempt_cnt_inc()						COUNT_INC(g_sched_preempt_cnt)

#ifdef CONFIG_PREEMPT_NESTING
#define mrt_preempt_enable()						mrt_barrier()
#define mrt_preempt_disable()						mrt_barrier()
#define mrt_preempt_is_locked()						(!!g_sched_preempt_cnt)

#else
#define mrt_preempt_enable()	\
	do {	\
		mrt_barrier();	\
		mrt_preempt_cnt_dec();	\
	} while (0)

#define mrt_preempt_disable()	\
	do {	\
		mrt_preempt_cnt_inc();	\
		mrt_barrier();	\
	} while (0)

#define mrt_preempt_is_locked()						(!!g_sched_preempt_cnt)
#endif



#endif /* __KERNEL_H */
