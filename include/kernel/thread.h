/*
 * Kernel Thread Management Defines
 *
 * File Name:   thread.c
 * Author:      Yang Yujun
 * E-mail:      <yujiantianhu@163.com>
 * Created on:  2023.07.09
 *
 * Copyright (c) 2023   Yang Yujun <yujiantianhu@163.com>
 *
 */

#ifndef __THREAD_H_
#define __THREAD_H_

#ifdef __cplusplus
    extern "C" {
#endif

/*!< The includes */
#include <common/generic.h>
#include <common/list_types.h>
#include <common/api_string.h>
#include <common/io_stream.h>
#include <common/time.h>
#include <board/board.h>
#include <boot/boot_text.h>
#include <boot/implicit_call.h>
#include <platform/fwk_mempool.h>
#include <kernel/kernel.h>
#include <kernel/context.h>
#include <kernel/sleep.h>

/*!< The defines */
typedef kint32_t tid_t;

/*!< maximum number of threads that can be created */
#define THREAD_MAX_NUM						(1024)

/*!< minimum space for thread stack (unit: byte) */
#define THREAD_STACK8(byte)				    (mrt_align4(byte) >> 0)
#define THREAD_STACK16(half)				(mrt_align4(half) >> 1)
#define THREAD_STACK32(word)				(mrt_align4(word) >> 2)

/*!< 1 page = 4 kbytes; half page = (1 / 2) page; quarter = (1 / 4) page */
#define THREAD_STACK_PAGE(page)			    (THREAD_STACK8(((kuint32_t)(page)) << 12))
#define THREAD_STACK_HALF(page)			    (THREAD_STACK8(((kuint32_t)(page)) << 11))
#define THREAD_STACK_QUAR(page)			    (THREAD_STACK8(((kuint32_t)(page)) << 10))

#define THREAD_STACK_MIN					THREAD_STACK8(512)
#define THREAD_STACK_DEFAULT				THREAD_STACK8(2056)

/*!<
 * tid base 
 * 0 ~ 31: fixed tid for kernel thread;
 * 32 ~ 127: dynamic tid for kernel thread;
 * 128 ~ THREAD_MAX_NUM: user thread
 */
#define THREAD_TID_IDLE                     (0)                 /*!< idle thread */
#define THREAD_TID_BASE                     (1)                 /*!< kernel thread (parent) */
#define THREAD_TID_INIT                     (2)                 /*!< init thread */

#define THREAD_TID_SOCKRX                   (19)
#define THREAD_TID_SOCKTX                   (20)

#define THREAD_TID_DYNC                     (32)                /*!< dynamic kernel thread */
#define THREAD_TID_USER					    (128)               /*!< user/application thread */

/*!<
 * priority
 * kernel thread requires higher priority (0 ~ 19)
 * The lower the value, the higher the priority
 */
#define THREAD_PROTY_START					(99)
#define THREAD_PROTY_DEFAULT				(80)
#define THREAD_PROTY_MAX					(0)

#define THREAD_PROTY_IDLE				    (98)
#define THREAD_PROTY_KERNEL				    (THREAD_PROTY_MAX)
#define THREAD_PROTY_INIT					(THREAD_PROTY_KERNEL + 1)
#define THREAD_PROTY_KWORKER				(THREAD_PROTY_KERNEL + 1)

#define THREAD_PROTY_TERM                   (THREAD_PROTY_DEFAULT)

#define THREAD_PROTY_SOCKRX                 (19)
#define THREAD_PROTY_SOCKTX                 (20)

#define __THREAD_IS_LOW_PRIO(prio, prio2)	((prio2) <= (prio))
#define __THREAD_HIGHER_DEFAULT(val)		(THREAD_PROTY_DEFAULT - (val))	

/*!< preempt period */
#define THREAD_PREEMPT_PERIOD               (20)                /*!< unit: ms */

/*!< time slice */
#define THREAD_TIME_DEFUALT                 (40)				/*!< unit: ms */

/*!< policy */
enum __ERT_THREAD_DETACH
{
    /*!< joinable */
    THREAD_CREATE_JOINABLE = 0,

    /*!< detached */
    THREAD_CREATE_DETACHED
};

/*!<
 * inheritance policy
 * whether to inherit the attributes of the parent thread, this setting takes effect during the thread creation phase
 */
enum __ERT_THREAD_SCHED
{
    /*!< inherit */
    THREAD_INHERIT_SCHED = 0,

    /*!< explicit */
    THREAD_EXPLICIT_SCHED
};

/*!< schedule policy */
enum __ERT_THREAD_POLICY
{
    /*!< normal */
    THREAD_SCHED_OTHER = 0,

    /*!< preemptive schedule */
    THREAD_SCHED_FIFO,

    /*!< polling schedule */
    THREAD_SCHED_RR
};

struct scheduler_param
{
    kint32_t sched_priority;
    kint32_t sched_curpriority;

    union
    {
        kint32_t reserved[8];

        struct
        {
            kint32_t __ss_low_priority;
            kint32_t __ss_max_repl;
            struct time_spec __ss_repl_period;
            struct time_spec __ss_init_budget;

        } sgrt_ss;

    } ugrt_ss;

#define mrt_sched_low_priority   			    ugrt_ss.sgrt_ss.__ss_low_priority
#define mrt_sched_max_repl					    ugrt_ss.sgrt_ss.__ss_max_repl
#define mrt_sched_repl_period				    ugrt_ss.sgrt_ss.__ss_repl_period
#define mrt_sched_init_budget				    ugrt_ss.sgrt_ss.__ss_init_budget
};

struct thread_attr
{
    kint32_t detachstate;                       /*!< refer to "__ERT_THREAD_DETACH" */
    kint32_t schedpolicy;                       /*!< refer to "__ERT_THREAD_SCHED" */
    struct scheduler_param sgrt_param;          /*!< schedule parameters */
    kint32_t inheritsched;                      /*!< refer to "__ERT_THREAD_POLICY" */
    kint32_t scope;                             /*!< the scope of threads */
    kssize_t guardsize;                         /*!< the size of the alert buffer at the end of the thread stack */

    void *ptr_stack_start;                      /*!< thread stack address base (from dynamic allocation) */
    kutype_t stack_addr;                        /*!< thread stack top, 8 byte anlignment  */
    kusize_t stacksize;                         /*!< thread stack size (unit: byte), the minimum can be set to THREAD_STACK_MIN */

    struct mem_info sgrt_pool;                  /*!< thread memory pool */
};
typedef struct thread_attr srt_thread_attr_t;

/*!< The defines */
extern tid_t kernel_thread_create(tid_t tid, 
                                        struct thread_attr *sprt_attr, 
                                        void *(*pfunc_start_routine) (void *), 
                                        void *ptr_args);

extern kint32_t thread_create(tid_t *ptr_id, 
                                        struct thread_attr *sprt_attr, 
                                        void *(*pfunc_start_routine) (void *), 
                                        void *ptr_args);

extern kint32_t kernel_thread_idle_create(struct thread_attr *sprt_attr, 
                                        void *(*pfunc_start_routine) (void *), 
                                        void *ptr_args);

extern kint32_t kernel_thread_base_create(struct thread_attr *sprt_attr, 
                                        void *(*pfunc_start_routine) (void *), 
                                        void *ptr_args);
                                                
extern kint32_t kernel_thread_init_create(struct thread_attr *sprt_attr, 
                                        void *(*pfunc_start_routine) (void *), 
                                        void *ptr_args);

extern void *thread_attr_init(struct thread_attr *sprt_attr);
extern void *thread_attr_revise(struct thread_attr *sprt_attr);
extern void thread_attr_destroy(struct thread_attr *sprt_attr);
extern struct thread_attr *thread_attr_get(tid_t tid);
extern void *thread_set_stack(struct thread_attr *sprt_attr, 
                                    void *ptr_dync, void *ptr_stack, kusize_t stacksize);

extern kint32_t thread_create_mempool(struct thread_attr *sprt_attr, void *base, kusize_t size);
extern void thread_release_mempool(struct thread_attr *sprt_attr);
extern void *tmalloc(size_t __size, nrt_gfp_t flags);
extern void *tcalloc(size_t __size, size_t __n, nrt_gfp_t flags);
extern void *tzalloc(size_t __size, nrt_gfp_t flags);
extern void tfree(void *__ptr);

/*!< API functions */
/*!
 * @brief	get current priority
 * @param  	sprt_attr
 * @retval 	priority
 * @note   	none
 */
static inline kuint32_t thread_get_priority(struct thread_attr *sprt_attr)
{
    return sprt_attr->sgrt_param.sched_curpriority;
}

/*!
 * @brief	set priority
 * @param  	sprt_attr, priority
 * @retval 	none
 * @note   	none
 */
static inline void thread_set_priority(struct thread_attr *sprt_attr, kuint32_t priority)
{
    sprt_attr->sgrt_param.sched_priority = __THREAD_IS_LOW_PRIO(priority, THREAD_PROTY_MAX) ? priority : THREAD_PROTY_MAX;
    sprt_attr->sgrt_param.sched_curpriority = sprt_attr->sgrt_param.sched_priority;
}

/*!
 * @brief	set time slice
 * @param  	sprt_attr, time
 * @retval 	none
 * @note   	none
 */
static inline void thread_set_time_slice(struct thread_attr *sprt_attr, kutime_t mseconds)
{
    struct time_spec sgrt_tm;
    
    msecs_to_time_spec(&sgrt_tm, mseconds);
    memcpy(&sprt_attr->sgrt_param.mrt_sched_init_budget, &sgrt_tm, sizeof(sgrt_tm));
}

/*!
 * @brief	get time slice (to milseconds)
 * @param  	sprt_attr
 * @retval 	milseconds
 * @note   	none
 */
static inline kuint32_t thread_get_sched_msecs(struct thread_attr *sprt_attr)
{
    return time_spec_to_msecs(&sprt_attr->sgrt_param.mrt_sched_init_budget);
}

/*!
 * @brief	set stack size
 * @param  	sprt_attr, stacksize
 * @retval 	none
 * @note   	stack-size of each thread must more than THREAD_STACK_MIN
 */
static inline void thread_attr_setstacksize(struct thread_attr *sprt_attr, kusize_t stacksize)
{
    sprt_attr->stacksize = (stacksize >= THREAD_STACK_MIN) ? stacksize : THREAD_STACK_MIN;
}

/*!
 * @brief	get stack size
 * @param  	sprt_attr
 * @retval 	stack size
 * @note   	none
 */
static inline kuint32_t thread_attr_getstacksize(struct thread_attr *sprt_attr)
{
    return sprt_attr->stacksize;
}

/*!
 * @brief	get context
 * @param  	sprt_attr
 * @retval 	context structure
 * @note   	none
 */
static inline struct scheduler_context_regs *thread_get_context(struct thread_attr *sprt_attr)
{
    kutype_t base;

    base = sprt_attr->stack_addr + sizeof(struct scheduler_context_regs);
    base = mrt_align(base, 8) - sizeof(struct scheduler_context_regs);
    
    return (struct scheduler_context_regs *)base;
}

/*!
 * @brief	get address of stack_addr
 * @param  	sprt_attr
 * @retval 	&sprt_attr->stack_addr
 * @note   	stack = *(&sprt_attr->stack_addr) (excluding scheduler_context_regs)
 */
static inline kutype_t thread_get_stack(struct thread_attr *sprt_attr)
{
    return (kutype_t)(&sprt_attr->stack_addr);
}

/*!
 * @brief	set detach state
 * @param  	sprt_attr, detach state
 * @retval 	none
 * @note   	none
 */
static inline void thread_attr_setdetachstate(struct thread_attr *sprt_attr, kuint32_t state)
{
    sprt_attr->detachstate = state;
}

/*!
 * @brief	get detach state
 * @param  	sprt_attr
 * @retval 	detach state
 * @note   	none
 */
static inline kuint32_t thread_attr_getdetachstate(struct thread_attr *sprt_attr)
{
    return sprt_attr->detachstate;
}

/*!
 * @brief	set inherit policy
 * @param  	sprt_attr, sched
 * @retval 	none
 * @note   	none
 */
static inline void thread_attr_setinheritsched(struct thread_attr *sprt_attr, kuint32_t sched)
{
    sprt_attr->inheritsched	= sched;
}

/*!
 * @brief	get inherit policy
 * @param  	sprt_attr
 * @retval 	inherit policy
 * @note   	none
 */
static inline kuint32_t thread_attr_getinheritsched(struct thread_attr *sprt_attr)
{
    return sprt_attr->inheritsched;
}

/*!
 * @brief	set schedule policy
 * @param  	sprt_attr, schedule policy
 * @retval 	none
 * @note   	none
 */
static inline void thread_attr_setschedpolicy(struct thread_attr *sprt_attr, kuint32_t policy)
{
    sprt_attr->schedpolicy = policy;
}

/*!
 * @brief	set schedule policy
 * @param  	sprt_attr
 * @retval 	schedule policy
 * @note   	none
 */
static inline kuint32_t thread_attr_getschedpolicy(struct thread_attr *sprt_attr)
{
    return sprt_attr->schedpolicy;
}

/*!
 * @brief	set schedule parameters
 * @param  	sprt_attr, sprt_param
 * @retval 	none
 * @note   	copy param to attribute
 */
static inline void thread_attr_setschedparam(struct thread_attr *sprt_attr, struct scheduler_param *sprt_param)
{
    memcpy(&sprt_attr->sgrt_param, sprt_param, sizeof(struct scheduler_param));
}

/*!
 * @brief	get schedule parameters
 * @param  	sprt_attr, sprt_param
 * @retval 	none
 * @note   	copy param from attribute
 */
static inline void thread_attr_getschedparam(struct thread_attr *sprt_attr, struct scheduler_param *sprt_param)
{
    memcpy(sprt_param, &sprt_attr->sgrt_param, sizeof(struct scheduler_param));
}

#ifdef __cplusplus
    }
#endif

#endif /* _THREAD_H_ */
