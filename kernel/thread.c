/*
 * Thread Management Interface
 *
 * File Name:   thread.c
 * Author:      Yang Yujun
 * E-mail:      <yujiantianhu@163.com>
 * Created on:  2023.07.09
 *
 * Copyright (c) 2023   Yang Yujun <yujiantianhu@163.com>
 *
 */

/*!< The includes */
#include <kernel/thread.h>
#include <kernel/sched.h>

/*!< API functions */
/*!
 * @brief	create a new thread
 * @param  	...
 * @retval 	err code
 * @note   	configure attribute and thread
 */
static kint32_t __real_thread_create(real_thread_t *ptr_id, kint32_t base, struct real_thread_attr *sprt_attr,
                                    void *(*pfunc_start_routine) (void *), void *ptr_args, kuint32_t flags)
{
    real_thread_t tid;
    struct real_thread *sprt_thread;
    struct real_thread_attr *sprt_it_attr;
    kuint32_t i_start, count;
    kint32_t retval;

    /*!< check if is user thread */
    if ((REAL_THREAD_USER & flags) == REAL_THREAD_USER)
    {
        i_start = REAL_THREAD_TID_START;
        count	= REAL_THREAD_MAX_NUM - REAL_THREAD_TID_START;
    }
    /*!< kernel thread */
    else
    {
        i_start = 0;
        count	= REAL_THREAD_TID_START;
    }
    
    if ((base >= i_start) && (base < (i_start + count)))
    {
        i_start = base;
        count   = 1;
    }

    /*!< find a free tid */
    tid = get_unused_tid_from_scheduler(i_start, count);
    if (tid < 0)
        goto fail;

    sprt_it_attr = sprt_attr;

    if (!sprt_attr)
    {
        sprt_it_attr = (struct real_thread_attr *)kzalloc(sizeof(struct real_thread_attr), GFP_KERNEL);
        if (!isValid(sprt_it_attr))
            goto fail;

        /*!< initialize attr */
        if (!real_thread_attr_init(sprt_it_attr))
            goto fail2;
    }

    /*!< check if attr valid */
    if (!real_thread_attr_revise(sprt_it_attr))
        goto fail3;

    /*!< create new dynamic thread */
    sprt_thread = (struct real_thread *)kzalloc(sizeof(struct real_thread), GFP_KERNEL);
    if (!isValid(sprt_thread))
        goto fail3;

    sprt_thread->tid 			= tid;
    sprt_thread->sprt_attr 		= sprt_it_attr;
    sprt_thread->start_routine 	= pfunc_start_routine;
    sprt_thread->ptr_args		= ptr_args;

    /*!< add to ready list */
    retval = register_new_thread(sprt_thread, tid);
    if (retval < 0)
        goto fail4;

    if (ptr_id)
        *ptr_id = tid;
    
    return ER_NORMAL;

fail4:
    kfree(sprt_thread);
fail3:
    real_thread_attr_destroy(sprt_it_attr);
fail2:
    if (!isValid(sprt_attr))
        kfree(sprt_it_attr);
fail:
    return -ER_FAULT;
}

/*!
 * @brief	create kernel thread
 * @param  	...
 * @retval 	err code
 * @note   	none
 */
kint32_t __kernel_thread_create(real_thread_t *ptr_id, kint32_t base, 
                        struct real_thread_attr *sprt_attr, void *(*pfunc_start_routine) (void *), void *ptr_args)
{
    return __real_thread_create(ptr_id, base, 
                                sprt_attr, pfunc_start_routine, ptr_args, 0);
}

/*!
 * @brief	create user thread
 * @param  	...
 * @retval 	err code
 * @note   	none
 */
kint32_t __real_user_thread_create(real_thread_t *ptr_id, kint32_t base, 
                        struct real_thread_attr *sprt_attr, void *(*pfunc_start_routine) (void *), void *ptr_args)
{
    return __real_thread_create(ptr_id, base, 
                                sprt_attr, pfunc_start_routine, ptr_args, REAL_THREAD_USER);
}

/*!
 * @brief	create kernel thread
 * @param  	...
 * @retval 	err code
 * @note   	none
 */
kint32_t kernel_thread_create(real_thread_t *ptr_id, struct real_thread_attr *sprt_attr, 
                        void *(*pfunc_start_routine) (void *), void *ptr_args)
{
    return __kernel_thread_create(ptr_id, -1, 
                                sprt_attr, pfunc_start_routine, ptr_args);
}

/*!
 * @brief	create user thread
 * @param  	...
 * @retval 	err code
 * @note   	none
 */
kint32_t real_thread_create(real_thread_t *ptr_id, struct real_thread_attr *sprt_attr, 
                        void *(*pfunc_start_routine) (void *), void *ptr_args)
{
    return __real_user_thread_create(ptr_id, -1, 
                                sprt_attr, pfunc_start_routine, ptr_args);
}

/*!
 * @brief	create idle thread
 * @param  	...
 * @retval 	err code
 * @note   	tid is fixed to REAL_THREAD_TID_IDLE
 */
kint32_t kernel_thread_idle_create(struct real_thread_attr *sprt_attr, 
                                void *(*pfunc_start_routine) (void *), void *ptr_args)
{  
    return __kernel_thread_create(mrt_nullptr, REAL_THREAD_TID_IDLE, 
                                sprt_attr, pfunc_start_routine, ptr_args);
}

/*!
 * @brief	create base thread
 * @param  	...
 * @retval 	err code
 * @note   	tid is fixed to REAL_THREAD_TID_BASE
 */
kint32_t kernel_thread_base_create(struct real_thread_attr *sprt_attr, 
                                void *(*pfunc_start_routine) (void *), void *ptr_args)
{  
    return __kernel_thread_create(mrt_nullptr, REAL_THREAD_TID_BASE, 
                                sprt_attr, pfunc_start_routine, ptr_args);
}

/*!
 * @brief	create init thread
 * @param  	...
 * @retval 	err code
 * @note   	tid is fixed to REAL_THREAD_TID_INIT
 */
kint32_t kernel_thread_init_create(struct real_thread_attr *sprt_attr, 
                                void *(*pfunc_start_routine) (void *), void *ptr_args)
{  
    return __kernel_thread_create(mrt_nullptr, REAL_THREAD_TID_INIT, 
                                sprt_attr, pfunc_start_routine, ptr_args);
}

/*!
 * @brief	initial attribute
 * @param  	sprt_attr: attr
 * @retval 	none
 * @note   	none
 */
void *real_thread_attr_init(struct real_thread_attr *sprt_attr)
{
    void *ptr_stack;

    memset(sprt_attr, 0, sizeof(struct real_thread_attr));

    /*!< set default parmeters */
    /*!< detach state: join */
    sprt_attr->detachstate = REAL_THREAD_CREATE_JOINABLE;
    /*!< inherit policy: inherit */
    sprt_attr->inheritsched	= REAL_THREAD_INHERIT_SCHED;
    /*!< schedule policy: preempt */
    sprt_attr->schedpolicy = REAL_THREAD_SCHED_FIFO;

    /*!< stack: 128bytes */
    ptr_stack = kzalloc(REAL_THREAD_STACK_DEFAULT, GFP_KERNEL);
    if (!isValid(ptr_stack))
        return mrt_nullptr;

    real_thread_set_stack(sprt_attr, ptr_stack, ptr_stack, REAL_THREAD_STACK_DEFAULT);
    real_thread_set_priority(sprt_attr, REAL_THREAD_PROTY_DEFAULT);
    real_thread_set_time_slice(sprt_attr, REAL_THREAD_TIME_DEFUALT);

    return (void *)sprt_attr->stack_addr;
}

/*!
 * @brief	check attribute
 * @param  	sprt_attr: attr
 * @retval 	none
 * @note   	none
 */
void *real_thread_attr_revise(struct real_thread_attr *sprt_attr)
{
    void *ptr_stack;

    if (!sprt_attr)
        return mrt_nullptr;

    if (!sprt_attr->sgrt_param.sched_priority)
        real_thread_set_priority(sprt_attr, REAL_THREAD_PROTY_DEFAULT);
    
    if (mrt_is_timespec_empty(&sprt_attr->sgrt_param.mrt_sched_init_budget))
        real_thread_set_time_slice(sprt_attr, REAL_THREAD_TIME_DEFUALT);

    if (!sprt_attr->stack_addr)
    {
        /*!< stack: 128bytes */
        ptr_stack = kzalloc(REAL_THREAD_STACK_DEFAULT, GFP_KERNEL);
        if (!isValid(ptr_stack))
            return mrt_nullptr;		

        real_thread_set_stack(sprt_attr, ptr_stack, ptr_stack, REAL_THREAD_STACK_DEFAULT);
    }

    return (void *)sprt_attr->stack_addr;
}

/*!
 * @brief	destroy attribute
 * @param  	sprt_attr: attr
 * @retval 	none
 * @note   	none
 */
void real_thread_attr_destroy(struct real_thread_attr *sprt_attr)
{
    if (!sprt_attr)
        return;

    if (sprt_attr->ptr_stack_start)
        kfree(sprt_attr->ptr_stack_start);
    
    memset(sprt_attr, 0, sizeof(struct real_thread_attr));
}

/*!
 * @brief	set stack
 * @param  	sprt_attr: thread attibute
 * @param   ptr_dync: stack which allocated by dynamic (kmalloc/malloc)
 * @param   ptr_stack: stack base (low address)
 * @param   stacksize: total size of stack
 * @retval 	stack address (top of stack)
 * @note   	if stack is allocated by mempool, ptr_dync should set to be ptr_stack, 
 *          so that it can be released by sprt_attr->ptr_stack_start
 */
void *real_thread_set_stack(struct real_thread_attr *sprt_attr, 
                                    void *ptr_dync, void *ptr_stack, kusize_t stacksize)
{
    struct scheduler_context_regs *sprt_regs;

    if (!isValid(ptr_stack) || (stacksize < REAL_THREAD_STACK_MIN))
        return mrt_nullptr;

    /*!< check: ptr_dync just should be NULL or ptr_stack */
    if (ptr_dync && (ptr_dync != ptr_stack))
        return mrt_nullptr;

    /*!< 
     * if the stack is defined in a static storage area, ptr_dync should be NULL; 
     * Otherwise, the address of ptr_stack should be passed in 
     */
    sprt_attr->ptr_stack_start = ptr_dync;

    /*!< pointer to stack top with 8 bytes alignment */
    sprt_attr->stack_addr = (kutype_t)((kuint8_t *)ptr_stack + stacksize);
    sprt_attr->stack_addr = mrt_ralign(sprt_attr->stack_addr - 16, 8);
    sprt_attr->stack_addr -= sizeof(struct scheduler_context_regs);
    sprt_attr->stack_addr = mrt_ralign(sprt_attr->stack_addr, 8);
    sprt_attr->stacksize = stacksize;

    sprt_regs = real_thread_get_context(sprt_attr);
    memset(sprt_regs, 0, sizeof(struct scheduler_context_regs));

    return (void *)sprt_attr->stack_addr;
}

/*!
 * @brief   create memory pool
 * @param   sprt_attr, base, size
 * @retval  none
 * @note    thread memory block create
 */
kint32_t real_thread_create_mempool(struct real_thread_attr *sprt_attr, void *base, kusize_t size)
{
    if (!isValid(base))
        return PTR_ERR(base);

    /*!< must 8-byte alignment */
    if (((kuaddr_t)base) & 0x07)
        return -ER_NOTALIGN;
    
    return memory_simple_block_create(&sprt_attr->sgrt_pool, (kuaddr_t)base, size);
}

/*!
 * @brief   destroy memory
 * @param   sprt_attr
 * @retval  none
 * @note    thread memory block destroy
 */
void real_thread_release_mempool(struct real_thread_attr *sprt_attr)
{
    if (sprt_attr->sgrt_pool.sprt_mem)
        memory_simple_block_destroy(&sprt_attr->sgrt_pool);
}

/*!
 * @brief   tmalloc
 * @param   __size
 * @retval  none
 * @note    thread memory pool allocate
 */
void *tmalloc(size_t __size, ert_fwk_mempool_t flags)
{
    struct real_thread *sprt_thread = mrt_current;
    struct mem_info *sprt_info;
    void *p = mrt_nullptr;

    if (!sprt_thread)
        goto END;

    if (GFP_KERNEL != (flags & GFP_KERNEL))
        goto END;

    sprt_info = &sprt_thread->sprt_attr->sgrt_pool;
    p = alloc_spare_simple_memory(sprt_info->sprt_mem, __size);
    if (!isValid(p))
        return mrt_nullptr;

    if (flags & NR_KMEM_ZERO)
        memory_reset(p, __size);

END:
    return p;
}

/*!
 * @brief   tcalloc
 * @param   __size, __n
 * @retval  none
 * @note    thread memory pool allocate (array)
 */
void *tcalloc(size_t __size, size_t __n, ert_fwk_mempool_t flags)
{
    return tmalloc(__size * __n, flags);
}

/*!
 * @brief   tzalloc
 * @param   __size
 * @retval  none
 * @note    thread memory pool allocate, and reset automatically
 */
void *tzalloc(size_t __size, ert_fwk_mempool_t flags)
{
    return tmalloc(__size, flags | GFP_ZERO);
}

/*!
 * @brief   tfree
 * @param   __ptr
 * @retval  none
 * @note    thread memory pool free
 */
void tfree(void *__ptr)
{
    struct real_thread *sprt_thread = mrt_current;
    struct mem_info *sprt_info;

    sprt_info = &sprt_thread->sprt_attr->sgrt_pool;
    if ((__ptr <  (void *)sprt_info->base) ||
        (__ptr >= (void *)(sprt_info->base + sprt_info->lenth)))
        return;

    free_employ_simple_memory(sprt_info->sprt_mem, __ptr);
}

/*!< end of file */
